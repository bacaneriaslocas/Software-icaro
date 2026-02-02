#include <stdint.h>
#include <stdbool.h>
#include "stm32h7xx_hal.h"

/* TAREAS PENDIENTES:
 *
 * Configurar las interrupciones para hacer mas eficiente la transferencia de datos
 *
 * Dar una opcion en forma de funcion para configurar los bits de control del dispositivo
 *
 */

// Constantes del MMC5983MA
#define MMC5983_ZERO     131072    // 2^17: punto medio (18 bits sin signo)
#define MMC5983_LSB_uT   0.025f    // 1 LSB = 0.025 µT (0.25 mG)

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

typedef struct {
	uint32_t x;
	uint32_t y;
	uint32_t z;
} mmc5983_raw_axes_t;

typedef struct {
	float x;
	float y;
	float z;
} mmc5983_axes_t;

bool mmc_init(SPI_HandleTypeDef *spi, GPIO_TypeDef *CS_port, uint16_t CS_pin);		// leemos la id del sensor y configuramos el modo de operacion
uint8_t mmc_read_reg(uint8_t reg);
void mmc_write_reg(uint8_t reg, uint8_t val);

mmc5983_axes_t mmc5983_read_xyz(void);



