#pragma once
#include "stm32h7xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

// =======================
// Tipos compatibles con tu código
// =======================

typedef struct {
    float x;
    float y;
    float z;
} icm_accel_t;

typedef struct {
    float x;
    float y;
    float z;
} icm_gy_t;

// =======================
// CONFIG ENUMS (según PDF)
// =======================

// ACC_RANGE (0x41) FS[1:0]
typedef enum {
    ICM_ACC_FS_2G  = 0x00,
    ICM_ACC_FS_4G  = 0x01,
    ICM_ACC_FS_8G  = 0x02,
    ICM_ACC_FS_16G = 0x03
} icm_acc_fs_t;

// GYR_RANGE (0x43) FS[2:0]
typedef enum {
    ICM_GYR_FS_2000DPS = 0x00,
    ICM_GYR_FS_1000DPS = 0x01,
    ICM_GYR_FS_500DPS  = 0x02,
    ICM_GYR_FS_250DPS  = 0x03,
    ICM_GYR_FS_125DPS  = 0x04
} icm_gyr_fs_t;

// ODR nibble (bits3:0) según tabla del PDF
typedef enum {
    ICM_ODR_0_78HZ  = 0x01,
    ICM_ODR_1_5HZ   = 0x02,
    ICM_ODR_3_125HZ = 0x03,
    ICM_ODR_6_25HZ  = 0x04,
    ICM_ODR_12_5HZ  = 0x05,
    ICM_ODR_25HZ    = 0x06,
    ICM_ODR_50HZ    = 0x07,
    ICM_ODR_100HZ   = 0x08,
    ICM_ODR_200HZ   = 0x09,
    ICM_ODR_400HZ   = 0x0A,
    ICM_ODR_800HZ   = 0x0B,
    ICM_ODR_1600HZ  = 0x0C,
    ICM_ODR_3200HZ  = 0x0D
} icm_odr_t;

// =======================
// API
// =======================
bool icm_init(SPI_HandleTypeDef *spi, GPIO_TypeDef *cs_port, uint16_t cs_pin);
bool icm_reset(void);

bool icm_config(icm_acc_fs_t acc_fs, icm_gyr_fs_t gyr_fs,
                icm_odr_t acc_odr, icm_odr_t gyr_odr);

uint8_t icm_read_reg(uint8_t reg);
void icm_write_reg(uint8_t reg, uint8_t val);

icm_accel_t icm_read_acc(void);
icm_gy_t icm_read_gy(void);

float icm_read_temp_c(void);