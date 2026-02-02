#include <stdint.h>
#include <stdbool.h>
#include "stm32h7xx_hal.h"
#include "main.h"

/*
 * REGISTROS ACELEROMETRO
 */

#define ACC_SOFTRESET 0x7E
#define ACC_PWR_CTRL 0x7D
#define ACC_PWR_CONF 0x7C
#define ACC_SELF_TEST 0x6D
#define INT_MAP_DATA 0x58
#define INT2_IO_CTRL 0x54
#define INT1_IO_CTRL 0x53
#define FIFO_CONFIG_1 0x49
#define FIFO_CONFIG_0 0x48
#define FIFO_WTM_1 0x47
#define FIFO_WTM_0 0x46
#define FIFO_DOWNS 0x45
#define ACC_RANGE 0x41
#define ACC_CONF 0x40
#define BMI_ACC_FIFO_DATA 0x26
#define FIFO_LENGTH_1 0x25
#define FIFO_LENGTH_0 0x24
#define TEMP_LSB 0x23
#define TEMP_MSB 0x22
#define ACC_INT_STAT_1 0x1D
#define SENSORTIME_2 0x1A
#define SENSORTIME_1 0x19
#define SENSORTIME_0 0x18
#define ACC_Z_MSB 0x17
#define ACC_Z_LSB 0x16
#define ACC_Y_MSB 0x15
#define ACC_Y_LSB 0x14
#define ACC_X_MSB 0x13
#define ACC_X_LSB 0x12
#define ACC_STATUS 0x03
#define ACC_ERR_REG 0x02
#define ACC_CHIP_ID 0x00

/*
 * REGISTROS GIROSCOPIO
 */

#define BMI_GY_FIFO_DATA 0x3F
#define BMI_FIFO_CONFIG_1 0x3E
#define BMI_FIFO_CONFIG_0 0x3D
#define GYRO_SELF_TEST 0x3C
#define FIFO_EXT_INT_S 0x34
#define FIFO_WN_EN 0x1E
#define INT3_INT4_IO_MAP 0x18
#define INT3_INT4_IO_CONF 0x16
#define GYRO_INT_CTRL 0x15
#define GYRO_SOFTRESET 0x14
#define GYRO_LPM1 0x11
#define GYRO_BANDWIDTH 0x10
#define GYRO_RANGE 0x0F
#define BMI_FIFO_STATUS 0x0E
#define GYRO_INT_STAT_1 0x0A
#define RATE_Z_MSB 0x07
#define RATE_Z_LSB 0x06
#define RATE_Y_MSB 0x05
#define RATE_Y_LSB 0x04
#define RATE_X_MSB 0x03
#define RATE_X_LSB 0x02
#define GYRO_CHIP_ID 0x00

#define SOFTRESET_CMD   0xB6

typedef struct {
    float x;
    float y;
    float z;
} bmi_accel_t;

typedef struct {
    float x;
    float y;
    float z;
} bmi_gy_t;



bool bmi_init(SPI_HandleTypeDef *spi, GPIO_TypeDef *ACC_CS_port, uint16_t ACC_CS_pin, GPIO_TypeDef *GY_CS_port, uint16_t GY_CS_pin);

bmi_accel_t bmi_read_acc(void);
bmi_gy_t bmi_read_gy(void);

uint8_t bmiacc_read_reg(uint8_t reg);
void bmiacc_write_reg(uint8_t reg, uint8_t val);

uint8_t bmigy_read_reg(uint8_t reg);
void bmigy_write_reg(uint8_t reg, uint8_t val);

