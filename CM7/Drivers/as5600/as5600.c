#import "as5600.h"

static HAL_StatusTypeDef as5600_read8(as5600_t *dev, uint8_t reg, uint8_t *val)
{
    return HAL_I2C_Mem_Read(dev->hi2c, dev->addr, reg, I2C_MEMADD_SIZE_8BIT, val, 1, 100);
}

static HAL_StatusTypeDef as5600_write8(as5600_t *dev, uint8_t reg, uint8_t val)
{
    return HAL_I2C_Mem_Write(dev->hi2c, dev->addr, reg, I2C_MEMADD_SIZE_8BIT, &val, 1, 100);
}

static HAL_StatusTypeDef as5600_read16(as5600_t *dev, uint8_t reg_high, uint16_t *out)
{
    // RAW_ANGLE, ANGLE, MAGNITUDE son 12-bit en dos bytes (high contiene bits 11:8) :contentReference[oaicite:4]{index=4}
    uint8_t b[2] = {0};
    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(dev->hi2c, dev->addr, reg_high, I2C_MEMADD_SIZE_8BIT, b, 2, 100);
    if (st != HAL_OK) return st;

    uint16_t v = ((uint16_t)(b[0] & 0x0F) << 8) | b[1]; // 12-bit
    *out = v;
    return HAL_OK;
}

HAL_StatusTypeDef as5600_init(as5600_t *dev, I2C_HandleTypeDef *hi2c, uint16_t addr)
{
    if (!dev || !hi2c) return HAL_ERROR;
    dev->hi2c = hi2c;
    dev->addr = (addr != 0) ? addr : AS5600_I2C_ADDR;

    // No hay “WHOAMI”, pero puedes comprobar que responde leyendo STATUS o RAW_ANGLE.
    uint8_t s = 0;
    return as5600_read8(dev, AS5600_REG_STATUS, &s);
}

HAL_StatusTypeDef as5600_read_raw_angle(as5600_t *dev, uint16_t *raw12)
{
    return as5600_read16(dev, AS5600_REG_RAW_ANGLE_H, raw12); // 0x0C/0x0D :contentReference[oaicite:5]{index=5}
}

HAL_StatusTypeDef as5600_read_angle(as5600_t *dev, uint16_t *angle12)
{
    return as5600_read16(dev, AS5600_REG_ANGLE_H, angle12); // 0x0E/0x0F :contentReference[oaicite:6]{index=6}
}

float as5600_angle_deg(uint16_t angle12)
{
    // 12-bit => 0..4095 -> 0..360°
    return (360.0f * (float)angle12) / 4096.0f;
}

HAL_StatusTypeDef as5600_read_status(as5600_t *dev, as5600_status_t *st)
{
    uint8_t v = 0;
    HAL_StatusTypeDef r = as5600_read8(dev, AS5600_REG_STATUS, &v);
    if (r != HAL_OK) return r;

    // STATUS bits: MD/ML/MH :contentReference[oaicite:7]{index=7}
    st->mh = (v & (1u << 3)) != 0;
    st->ml = (v & (1u << 4)) != 0;
    st->md = (v & (1u << 5)) != 0;
    return HAL_OK;
}

HAL_StatusTypeDef as5600_read_agc(as5600_t *dev, uint8_t *agc)
{
    return as5600_read8(dev, AS5600_REG_AGC, agc);
}

HAL_StatusTypeDef as5600_read_magnitude(as5600_t *dev, uint16_t *mag12)
{
    return as5600_read16(dev, AS5600_REG_MAG_H, mag12);
}

HAL_StatusTypeDef as5600_write_conf(as5600_t *dev, uint16_t conf14)
{
    // CONF son 14 bits repartidos en 0x07/0x08 (ver mapping) :contentReference[oaicite:8]{index=8}
    uint8_t high = (conf14 >> 8) & 0x3F; // bits 13:8
    uint8_t low  = conf14 & 0xFF;        // bits 7:0
    HAL_StatusTypeDef st = as5600_write8(dev, AS5600_REG_CONF_H, high);
    if (st != HAL_OK) return st;
    return as5600_write8(dev, AS5600_REG_CONF_L, low);
}

HAL_StatusTypeDef as5600_read_conf(as5600_t *dev, uint16_t *conf14)
{
    uint8_t b[2] = {0};
    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(dev->hi2c, dev->addr, AS5600_REG_CONF_H, I2C_MEMADD_SIZE_8BIT, b, 2, 100);
    if (st != HAL_OK) return st;

    *conf14 = (((uint16_t)b[0] & 0x3F) << 8) | b[1];
    return HAL_OK;
}