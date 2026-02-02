#include "lps22hb.h"

static SPI_HandleTypeDef *lps_spi;
static GPIO_TypeDef *lps_cs_port;
static uint16_t lps_cs_pin;

#define CS_LOW()   HAL_GPIO_WritePin(lps_cs_port, lps_cs_pin, GPIO_PIN_RESET)
#define CS_HIGH()  HAL_GPIO_WritePin(lps_cs_port, lps_cs_pin, GPIO_PIN_SET)


bool  lps_init(SPI_HandleTypeDef *spi, GPIO_TypeDef *CS_port, uint16_t CS_pin)
{

    uint8_t whoami;
    uint8_t reg = LPS_WHO_AM_I;

    lps_spi = spi;
    lps_cs_port = CS_port;
    lps_cs_pin = CS_pin;

    whoami = lps_read_reg(reg);

    lps_write_reg(CTRL_REG1, 0b01010010);   // configuracion del sensor
    lps_write_reg(CTRL_REG2, 0b00011000);
    lps_write_reg(CTRL_REG3, 0b00000100);

    return whoami == 0xB1;

}

float lps_read_pressure(void){

    // 2) Lee 3 bytes: PRESS_OUT_XL(0x28), _L(0x29), _H(0x2A) con auto-increment
    uint8_t tx[4] = { (uint8_t)(0x80 | 0x28), 0, 0, 0 }; // 0x80=read, 0x28=PRESS_OUT_XL
    uint8_t rx[4] = {0};

    CS_LOW();
    HAL_SPI_TransmitReceive(lps_spi, tx, rx, 4, 100);
    CS_HIGH();

    int32_t raw = ((int32_t)rx[3] << 16) | ((int32_t)rx[2] << 8) | rx[1];
    if (raw & 0x00800000) raw |= 0xFF000000; // sign-extend 24 bits

    // 4) Conversión: 1 LSB = 1/4096 hPa
    return (float)raw / 4096.0f;

}

float lps_read_temp(void){

    uint8_t tx[3] = { (uint8_t)(0x80 | 0x2B), 0, 0 }; // 0x80 = read + auto increment
    uint8_t rx[3] = {0};

    CS_LOW();
    HAL_SPI_TransmitReceive(lps_spi, tx, rx, 3, 100);
    CS_HIGH();

    // 3️⃣ Une los bytes a un entero de 16 bits (con signo)
    int16_t raw_temp = (int16_t)((rx[2] << 8) | rx[1]);

    // 4️⃣ Conversión según datasheet:
    //    TEMP(°C) = 42.5 + raw / 480
    float temperature = ((float)raw_temp / 100.0f);

    return temperature;
}

uint8_t lps_read_reg(uint8_t reg) {
    uint8_t tx[2] = { (uint8_t)(0x80 | reg), 0x00 };  // bit7=1 → lectura
    uint8_t rx[2] = {0};
    CS_LOW();
    HAL_SPI_TransmitReceive(lps_spi, tx, rx, 2, 100); // 1er byte: comando, 2º: dato recibido
    CS_HIGH();
    return rx[1];
}

void lps_write_reg(uint8_t reg, uint8_t val) {
    uint8_t tx[2] = { (uint8_t)(reg & 0x7F), val };
    CS_LOW();
    HAL_SPI_Transmit(lps_spi, tx, 2, 100);
    CS_HIGH();
}
