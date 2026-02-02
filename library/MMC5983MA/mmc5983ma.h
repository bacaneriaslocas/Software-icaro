#include <stdint.h>
#include "stm32h7xx_hal.h"


#define MMC_REG_XOUT0           	0x00
#define MMC_REG_XOUT1       		0x01
#define MMC_REG_YOUT0 				0x02
#define MMC_REG_YOUT1 				0x03
#define MMC_REG_ZOUT0 				0x04
#define MMC_REG_ZOUT1 				0x05
#define MMC_REG_XYZOUT2			0x06
#define MMC_REG_TOUT				0x07
#define MMC_REG_STATUS				0x08
#define MMC_REG_CTRL0				0x09
#define MMC_REG_CTRL1 				0x0A
#define MMC_REG_CTRL2				0x0B
#define MMC_REG_CTRL3				0x0C
#define MMC_REG_ID					0x2F

#define MMC_STATUS_MEAS_M_DONE   (1u << 0)

// Bits en CONTROL0 (0x09)
#define MMC_CTRL0_TM_M           (1u << 0)  // Trigger magnetic measurement



uint8_t  mmc_init(SPI_HandleTypeDef *spi, GPIO_TypeDef *CS_port, uint16_t CS_pin);		// leemos la id del sensor
uint8_t mmc_read_reg(uint8_t reg);
void mmc_write_reg(uint8_t reg, uint8_t val);

float mmc_read_x_mG(void);
uint32_t mmc_read_x_raw(void);

int getX(void);
int getY(void);
int getZ(void);

