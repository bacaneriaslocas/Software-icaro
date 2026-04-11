#include "icm42688p.h"

// Libreria adaptada al icm HXY, version china, no usar con icm de TDK, tienen
// registros diferentes


// =======================
// REGISTERS (según PDF)
// =======================
#define REG_WHO_AM_I     0x01

#define REG_ACC_X_H      0x0C
#define REG_ACC_Y_H      0x0E
#define REG_ACC_Z_H      0x10

#define REG_GYR_X_H      0x12
#define REG_GYR_Y_H      0x14
#define REG_GYR_Z_H      0x16

#define REG_TEMP_H       0x22

#define REG_ACC_CONF     0x40
#define REG_ACC_RANGE    0x41
#define REG_GYR_CONF     0x42
#define REG_GYR_RANGE    0x43

#define REG_SOFT_RST     0x4A
#define REG_PWR_CTRL     0x7D

#define REG_INT_CFG1     0x06

// =======================
// EXPECTED WHO_AM_I según PDF
// =======================
#define WHO_AM_I_EXPECTED 0x6A

// =======================
// GLOBALS
// =======================
static SPI_HandleTypeDef *icm_spi = NULL;
static GPIO_TypeDef *icm_cs_port = NULL;
static uint16_t icm_cs_pin = 0;

// Guardamos configuración actual para escalar bien
static icm_acc_fs_t current_acc_fs = ICM_ACC_FS_8G;
static icm_gyr_fs_t current_gyr_fs = ICM_GYR_FS_2000DPS;

#define CS_LOW()   HAL_GPIO_WritePin(icm_cs_port, icm_cs_pin, GPIO_PIN_RESET)
#define CS_HIGH()  HAL_GPIO_WritePin(icm_cs_port, icm_cs_pin, GPIO_PIN_SET)

// =======================
// LOW LEVEL SPI
// =======================
uint8_t icm_read_reg(uint8_t reg)
{
    uint8_t tx[2] = { (uint8_t)(0x80 | reg), 0x00 };
    uint8_t rx[2] = {0};

    CS_LOW();
    HAL_SPI_TransmitReceive(icm_spi, tx, rx, 2, 100);
    CS_HIGH();

    return rx[1];
}

void icm_write_reg(uint8_t reg, uint8_t val)
{
    uint8_t tx[2] = { (uint8_t)(reg & 0x7F), val };

    CS_LOW();
    HAL_SPI_Transmit(icm_spi, tx, 2, 100);
    CS_HIGH();
}

static bool icm_read_regs(uint8_t start_reg, uint8_t *buf, uint8_t len)
{
    if (len == 0) return false;

    uint8_t cmd = (uint8_t)(0x80 | start_reg);

    CS_LOW();

    if (HAL_SPI_Transmit(icm_spi, &cmd, 1, 100) != HAL_OK) {
        CS_HIGH();
        return false;
    }

    if (HAL_SPI_Receive(icm_spi, buf, len, 100) != HAL_OK) {
        CS_HIGH();
        return false;
    }

    CS_HIGH();
    return true;
}

// =======================
// RESET (según PDF)
// =======================
bool icm_reset(void)
{
    if (icm_spi == NULL) return false;

    // SOFT_RST (0x4A) escribir 0xA5
    icm_write_reg(REG_SOFT_RST, 0xA5);
    HAL_Delay(50);

    return true;
}

// =======================
// INIT
// =======================
bool icm_init(SPI_HandleTypeDef *spi, GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    icm_spi = spi;
    icm_cs_port = cs_port;
    icm_cs_pin = cs_pin;

    if (!icm_reset()) return false;

    uint8_t who = icm_read_reg(REG_WHO_AM_I);
    if (who != WHO_AM_I_EXPECTED) return false;

    // IMPORTANTE según PDF:
    // escribir 0x0E en PWR_CTRL (0x7D) y esperar 10ms
    icm_write_reg(REG_PWR_CTRL, 0x0E);
    HAL_Delay(10);

    // ==========================
    // CONFIG INTERRUPT INT1 DRDY
    // ==========================
    // INT_PP_OD = 0 (push-pull)
    // H_LACTIVE = 0 (active high)
    // INT1_SEL = 00011 (DRDY_GYR en INT1)
    // => 0b00000011
    icm_write_reg(REG_INT_CFG1, 0x03);

    // Config por defecto
    return icm_config(ICM_ACC_FS_8G, ICM_GYR_FS_2000DPS,
                      ICM_ODR_1600HZ, ICM_ODR_1600HZ);
}

// =======================
// CONFIG (solo registros del PDF)
// =======================
bool icm_config(icm_acc_fs_t acc_fs, icm_gyr_fs_t gyr_fs,
                icm_odr_t acc_odr, icm_odr_t gyr_odr)
{
    current_acc_fs = acc_fs;
    current_gyr_fs = gyr_fs;

    icm_write_reg(REG_ACC_RANGE, (uint8_t)(acc_fs & 0x03));
    icm_write_reg(REG_GYR_RANGE, (uint8_t)(gyr_fs & 0x07));

    // ACC_CONF:
    // bit7 = 1 high perf
    // BWP=010
    // ODR = acc_odr
    uint8_t acc_conf = 0;
    acc_conf |= (1 << 7);
    acc_conf |= (0b010 << 4);
    acc_conf |= (acc_odr & 0x0F);
    icm_write_reg(REG_ACC_CONF, acc_conf);

    // GYR_CONF:
    // bit7 = 1 high perf
    // noise=0
    // BWP=10
    // ODR = gyr_odr
    uint8_t gyr_conf = 0;
    gyr_conf |= (1 << 7);
    gyr_conf |= (0 << 6);
    gyr_conf |= (0b10 << 4);
    gyr_conf |= (gyr_odr & 0x0F);
    icm_write_reg(REG_GYR_CONF, gyr_conf);

    HAL_Delay(2);
    return true;
}

// =======================
// SCALE FACTORS
// =======================
static float acc_lsb_to_g(void)
{
    // Sensibilidades típicas (mg/LSB):
    // ±2g  = 0.061 mg/LSB
    // ±4g  = 0.122 mg/LSB
    // ±8g  = 0.244 mg/LSB
    // ±16g = 0.488 mg/LSB
    switch (current_acc_fs) {
        case ICM_ACC_FS_2G:  return 0.000061f;
        case ICM_ACC_FS_4G:  return 0.000122f;
        case ICM_ACC_FS_8G:  return 0.000244f;
        case ICM_ACC_FS_16G: return 0.000488f;
        default:             return 0.000244f;
    }
}

static float gyr_lsb_to_dps(void)
{
    // Sensibilidades típicas (mdps/LSB):
    // ±2000 = 61 mdps/LSB
    // ±1000 = 30.5
    // ±500  = 15.25
    // ±250  = 7.625
    // ±125  = 3.8125
    switch (current_gyr_fs) {
        case ICM_GYR_FS_2000DPS: return 0.061f;
        case ICM_GYR_FS_1000DPS: return 0.0305f;
        case ICM_GYR_FS_500DPS:  return 0.01525f;
        case ICM_GYR_FS_250DPS:  return 0.007625f;
        case ICM_GYR_FS_125DPS:  return 0.0038125f;
        default:                 return 0.061f;
    }
}

// =======================
// READ ACCEL (devuelve icm_accel_t)
// =======================
icm_accel_t icm_read_acc(void)
{
    icm_accel_t data = {0};
    uint8_t buf[6];

    if (!icm_read_regs(REG_ACC_X_H, buf, 6)) return data;

    int16_t x_raw = (int16_t)((buf[0] << 8) | buf[1]);
    int16_t y_raw = (int16_t)((buf[2] << 8) | buf[3]);
    int16_t z_raw = (int16_t)((buf[4] << 8) | buf[5]);

    float scale = acc_lsb_to_g();

    data.x = (float)x_raw * scale;
    data.y = (float)y_raw * scale;
    data.z = (float)z_raw * scale;

    return data;
}

// =======================
// READ GYRO (devuelve icm_gy_t)
// =======================
icm_gy_t icm_read_gy(void)
{
    icm_gy_t data = {0};
    uint8_t buf[6];

    if (!icm_read_regs(REG_GYR_X_H, buf, 6)) return data;

    int16_t x_raw = (int16_t)((buf[0] << 8) | buf[1]);
    int16_t y_raw = (int16_t)((buf[2] << 8) | buf[3]);
    int16_t z_raw = (int16_t)((buf[4] << 8) | buf[5]);

    float scale = gyr_lsb_to_dps();

    data.x = (float)x_raw * scale;
    data.y = (float)y_raw * scale;
    data.z = (float)z_raw * scale;

    return data;
}

// =======================
// TEMPERATURE
// =======================
float icm_read_temp_c(void)
{
    uint8_t buf[2];

    if (!icm_read_regs(REG_TEMP_H, buf, 2)) return 0.0f;

    int16_t raw = (int16_t)((buf[0] << 8) | buf[1]);

    // Según PDF:
    // T = raw/512 + 23°C
    return ((float)raw / 512.0f) + 23.0f;
}