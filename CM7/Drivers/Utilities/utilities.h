#ifndef UTILITIES_H
#define UTILITIES_H

#include "main.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal.h"
#include "gpio.h"

#include "tim.h"
#include "usart.h"
#include "adc.h"
#include "spi.h"
#include "i2c.h"

#include "vl53l1_platform.h"
#include "VL53L1X_api.h"
#include "lps22hb.h"
#include "icm42688p.h"
#include "mmc5983ma.h"
#include "bmi088.h"
#include "as5600.h"

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "shared.h"

#define RAW_TO_VOLTAGE 0.00005012588f // 3.3V/65535

#define CPU_P_ON()    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET)
#define CPU_P_OFF()    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET)
#define CPU_N_ON()    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET)
#define CPU_N_OFF()    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET)

#define SENS_P_ON()    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET)
#define SENS_P_OFF()    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET)
#define SENS_N_ON()    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET)
#define SENS_N_OFF()    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET)

#define BAT_P_ON()    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET)
#define BAT_P_OFF()    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET)
#define BAT_N_ON()    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET)
#define BAT_N_OFF()    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET)

#define RD_ON()    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, GPIO_PIN_SET)
#define RD_OFF()    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, GPIO_PIN_RESET)

#define GPS_ON()    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_SET)
#define GPS_OFF() HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_RESET)

#define SERVO_ON() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)

void led_test(void);
void Buzzer_on(uint32_t freq_hz);
void Buzzer_off(void);
void uart_print(const char *s);
void GPIO_Init(void);
float getDinamicPressure(void);
void Set_Servo_Angle(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t angle);
void sys_init(bool vervosity);
void mat3_mul(float A[3][3], float B[3][3], float C[3][3]);
void mat3_transfer(float A[3][3], float C[3][3]);

#endif /* UTILITIES_H */