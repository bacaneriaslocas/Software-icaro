#pragma once
#include "stm32h7xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

#ifndef AS5600_I2C_ADDR
#define AS5600_I2C_ADDR (0x36 << 1) // HAL usa 8-bit addr
#endif

// Register map (datasheet)
#define AS5600_REG_ZMCO        0x00
#define AS5600_REG_ZPOS_H      0x01
#define AS5600_REG_ZPOS_L      0x02
#define AS5600_REG_MPOS_H      0x03
#define AS5600_REG_MPOS_L      0x04
#define AS5600_REG_MANG_H      0x05
#define AS5600_REG_MANG_L      0x06
#define AS5600_REG_CONF_H      0x07
#define AS5600_REG_CONF_L      0x08

#define AS5600_REG_STATUS      0x0B
#define AS5600_REG_RAW_ANGLE_H 0x0C
#define AS5600_REG_RAW_ANGLE_L 0x0D
#define AS5600_REG_ANGLE_H     0x0E
#define AS5600_REG_ANGLE_L     0x0F

#define AS5600_REG_AGC         0x1A
#define AS5600_REG_MAG_H       0x1B
#define AS5600_REG_MAG_L       0x1C

#define AS5600_REG_BURN        0xFF

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint16_t addr;     // 8-bit address for HAL (0x36<<1)
} as5600_t;

typedef struct {
    bool md; // magnet detected
    bool ml; // magnet too weak
    bool mh; // magnet too strong
} as5600_status_t;

HAL_StatusTypeDef as5600_init(as5600_t *dev, I2C_HandleTypeDef *hi2c, uint16_t addr);

HAL_StatusTypeDef as5600_read_raw_angle(as5600_t *dev, uint16_t *raw12);
HAL_StatusTypeDef as5600_read_angle(as5600_t *dev, uint16_t *angle12);
float             as5600_angle_deg(uint16_t angle12);

HAL_StatusTypeDef as5600_read_status(as5600_t *dev, as5600_status_t *st);
HAL_StatusTypeDef as5600_read_magnitude(as5600_t *dev, uint16_t *mag12);
HAL_StatusTypeDef as5600_read_agc(as5600_t *dev, uint8_t *agc);

HAL_StatusTypeDef as5600_write_conf(as5600_t *dev, uint16_t conf14);
HAL_StatusTypeDef as5600_read_conf(as5600_t *dev, uint16_t *conf14);