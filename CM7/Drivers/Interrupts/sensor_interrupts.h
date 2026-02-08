/**
  ******************************************************************************
  * @file    sensor_interrupts.h
  * @brief   Header for sensor interrupt handlers and configuration
  ******************************************************************************
  * @attention
  * This module manages all sensor-related interrupts for the Icaro project
  ******************************************************************************
  */

#ifndef SENSOR_INTERRUPTS_H_
#define SENSOR_INTERRUPTS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "main.h"
#include <stdbool.h>

/* Global interrupt flags ====================================================*/
extern volatile bool icm_flag;      // ICM42688P interrupt triggered
extern volatile bool bmigy_flag;    // BMI088_GY interrupt triggered
extern volatile bool bmiacc_flag;   // BMI088_ACC interrupt triggered
extern volatile bool lps_flag;      // LPS22HB interrupt triggered
extern volatile bool mmc_flag;      // MMC5983MA interrupt triggered
extern volatile bool vl53_flag;     // VL53L1X interrupt triggered

/* Defines -------------------------------------------------------------------*/
/* Define your sensor interrupt pins and configurations here */
#define SENSOR_INT_PIN_1  GPIO_PIN_0
#define SENSOR_INT_PORT_1 GPIOA

/* Function Prototypes -------------------------------------------------------*/
/**
  * @brief Initialize all sensor interrupt handlers
  */
void sensor_interrupts_init(void);

/**
  * @brief Callback for sensor interrupt 1
  */
void sensor_interrupt_1_callback(void);

/**
  * @brief Callback for sensor interrupt 2
  */
void sensor_interrupt_2_callback(void);

/**
  * @brief EXTI interrupt handler for sensor interrupts
  */
void EXTI0_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_INTERRUPTS_H_ */
