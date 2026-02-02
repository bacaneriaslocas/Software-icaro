#include "mmc5983ma.h"     // si usas SPI del HAL

static SPI_HandleTypeDef *mmc_spi;
static GPIO_TypeDef *mmc_cs_port;
static uint16_t mmc_cs_pin;

#define CS_LOW()   HAL_GPIO_WritePin(mmc_cs_port, mmc_cs_pin, GPIO_PIN_RESET)
#define CS_HIGH()  HAL_GPIO_WritePin(mmc_cs_port, mmc_cs_pin, GPIO_PIN_SET)

uint8_t  mmc_init(SPI_HandleTypeDef *spi, GPIO_TypeDef *CS_port, uint16_t CS_pin)
{
    uint8_t whoami;
    uint8_t reg = MMC_REG_ID | 0x80;

    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(spi, &reg, 1, 100);
    HAL_SPI_Receive(spi, &whoami, 1, 100);
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_SET);

    return whoami;  // devolvemos la id del dispositivo ¡¡  cuidado con valores como 0x00 o 0xFF  !!
}

uint32_t mmc_read_x_raw(void)
{
    // 1) Dispara una medición de campo magnético
    mmc_write_reg(MMC_REG_CTRL0, MMC_CTRL0_TM_M);

    // 2) Espera a que termine (MEAS_M_DONE = 1)
    uint8_t st;
    do {
        st = mmc_read_reg(MMC_REG_STATUS);
    } while ((st & MMC_STATUS_MEAS_M_DONE) == 0);

    // (opcional) limpiar la bandera escribiendo 1; el datasheet indica que se limpia al iniciar la siguiente medida.

    // 3) Lee los registros de X y combina a 18 bits
    uint8_t x0   = mmc_read_reg(MMC_REG_XOUT0);   // X[17:10]
    uint8_t x1   = mmc_read_reg(MMC_REG_XOUT1);   // X[9:2]
    uint8_t xyz2 = mmc_read_reg(MMC_REG_XYZOUT2); // X[1:0] en bits [7:6]

    uint32_t x18 = ((uint32_t)x0 << 10)
                 | ((uint32_t)x1 << 2)
                 | ((uint32_t)(xyz2 >> 6) & 0x03);

    return x18; // unsigned, 0..262143 (18 bits)
}

float mmc_read_x_mG(void)
{
    uint32_t raw = mmc_read_x_raw();

    // 18-bit: 16384 counts/G → 1000 mG/G → 1000/16384 ≈ 0.061035 mG/LSB
    const float mG_per_count = 1000.0f / 16384.0f; // ≈ 0.061035 mG/LSB
    return raw * mG_per_count;
}

uint8_t mmc_read_reg(uint8_t reg) {
    uint8_t tx = reg | 0x80;  // bit7=1 -> read (ajusta si tu sensor difiere)
    uint8_t rx = 0;
    CS_LOW();
    HAL_SPI_Transmit(mmc_spi, &tx, 1, 100);
    HAL_SPI_Receive (mmc_spi, &rx, 1, 100);
    CS_HIGH();
    return rx;
}

void mmc_write_reg(uint8_t reg, uint8_t val) {
    uint8_t tx[2] = { (uint8_t)(reg & 0x7F), val };
    CS_LOW();
    HAL_SPI_Transmit(mmc_spi, tx, 2, 100);
    CS_HIGH();
}
