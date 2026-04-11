#include "mmc5983ma.h"     // si usas SPI del HAL

static SPI_HandleTypeDef *mmc_spi;
static GPIO_TypeDef *mmc_cs_port;
static uint16_t mmc_cs_pin;

#define CS_LOW()   HAL_GPIO_WritePin(mmc_cs_port, mmc_cs_pin, GPIO_PIN_RESET)

#define CS_HIGH()  HAL_GPIO_WritePin(mmc_cs_port, mmc_cs_pin, GPIO_PIN_SET)

// --- Calibración: matriz identidad y offset cero por defecto ---
static const float mag_offset[3] = {0.0f, 0.0f, 0.0f};
static const float mag_softiron[3][3] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f}
};


bool mmc_init(SPI_HandleTypeDef *spi, GPIO_TypeDef *CS_port, uint16_t CS_pin)
{
    uint8_t whoami;
    uint8_t reg = MMC_REG_ID;
    uint8_t valor = 0x00;

    mmc_spi = spi;
    mmc_cs_port = CS_port;
    mmc_cs_pin = CS_pin;

    whoami = mmc_read_reg(reg);

    HAL_Delay(3);


    reg = MMC_REG_CTRL0;
    valor = 0b00101100;
    mmc_write_reg(reg, valor);

    HAL_Delay(3);

    reg = MMC_REG_CTRL1;
    valor = 0b00000011; 
    mmc_write_reg(reg, valor);

  
    reg = MMC_REG_CTRL2;
    valor = 0b10111101;  
    mmc_write_reg(reg, valor);

    return whoami == 0x30;  // devolvemos la id del dispositivo ¡¡  cuidado con val ores como 0x00 o 0xFF  !!
}


mmc5983_axes_t mmc5983_read_xyz(void){
    mmc5983_axes_t data = {0};
    mmc5983_raw_axes_t rawData = {0};
    float raw_corr[3];
    float data_corr[3];
    uint8_t raw[7];

    for (uint8_t i = 0; i < 7; i++)
        raw[i] = mmc_read_reg(i); // direcciones 0x00–0x06

    rawData.x = ((uint32_t)raw[0] << 10) | ((uint32_t)raw[1] << 2) | ((raw[6] >> 6) & 0x03);
    rawData.y = ((uint32_t)raw[2] << 10) | ((uint32_t)raw[3] << 2) | ((raw[6] >> 4) & 0x03);
    rawData.z = ((uint32_t)raw[4] << 10) | ((uint32_t)raw[5] << 2) | ((raw[6] >> 2) & 0x03);

    // Conversión a microteslas
    raw_corr[0] = ((int32_t)rawData.x - MMC5983_ZERO) * MMC5983_LSB_uT - mag_offset[0];
    raw_corr[1] = ((int32_t)rawData.y - MMC5983_ZERO) * MMC5983_LSB_uT - mag_offset[1];
    raw_corr[2] = ((int32_t)rawData.z - MMC5983_ZERO) * MMC5983_LSB_uT - mag_offset[2];

    // Corrección soft iron: data_corr = mag_softiron * raw_corr
    for (int i = 0; i < 3; i++) {
        data_corr[i] = 0.0f;
        for (int j = 0; j < 3; j++) {
            data_corr[i] += mag_softiron[i][j] * raw_corr[j];
        }
    }

    data.x = data_corr[0];
    data.y = data_corr[1];
    data.z = data_corr[2];

    uint8_t reg = MMC_REG_ID;
    uint8_t valor = 0x00;

    reg = MMC_REG_STATUS;
    valor = 0b00000011;  
    mmc_write_reg(reg, valor);  // limpiamos el flag para que vuelva a saltar la interrupcion

    return data;
}

uint8_t mmc_read_reg(uint8_t reg) {
    uint8_t tx[2] = { (uint8_t)(0x80 | reg), 0x00 };  // bit7=1 → lectura
    uint8_t rx[2] = {0};
    CS_LOW();
    HAL_SPI_TransmitReceive(mmc_spi, tx, rx, 2, 100); // 1er byte: comando, 2º: dato recibido
    CS_HIGH();
    return rx[1];
}

void mmc_write_reg(uint8_t reg, uint8_t val) {
    uint8_t tx[2] = { (uint8_t)(reg & 0x7F), val };
    CS_LOW();
    HAL_SPI_Transmit(mmc_spi, tx, 2, 100);
    CS_HIGH();
}