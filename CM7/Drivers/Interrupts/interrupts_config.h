/**
  ******************************************************************************
  * @file    interrupts_config.h
  * @brief   Central configuration for all interrupt parameters
  ******************************************************************************
  */

#ifndef INTERRUPTS_CONFIG_H_
#define INTERRUPTS_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* GPIO Interrupt Pins ======================================================*/
// Sensor 1 Configuration
#define SENSOR_1_INT_PORT       GPIOA
#define SENSOR_1_INT_PIN        GPIO_PIN_0
#define SENSOR_1_INT_EDGE       GPIO_MODE_IT_FALLING

// Sensor 2 Configuration (ejemplo - puedes agregar más)
#define SENSOR_2_INT_PORT       GPIOB
#define SENSOR_2_INT_PIN        GPIO_PIN_1
#define SENSOR_2_INT_EDGE       GPIO_MODE_IT_FALLING

// Sensor 3 Configuration (BMI088, ICM42688P, etc.)
#define SENSOR_3_INT_PORT       GPIOC
#define SENSOR_3_INT_PIN        GPIO_PIN_2
#define SENSOR_3_INT_EDGE       GPIO_MODE_IT_FALLING

// Sensor 4 Configuration (Puerto E - ejemplo)
#define SENSOR_4_INT_PORT       GPIOE
#define SENSOR_4_INT_PIN        GPIO_PIN_5
#define SENSOR_4_INT_EDGE       GPIO_MODE_IT_FALLING

/* Interrupt Enable/Disable Flags ===========================================*/
#define ENABLE_SENSOR_1_INT     1
#define ENABLE_SENSOR_2_INT     1
#define ENABLE_SENSOR_3_INT     1
#define ENABLE_SENSOR_4_INT     1

/* Debounce Configuration ===================================================*/
#define INTERRUPT_DEBOUNCE_MS   10      // Debounce time in milliseconds

#ifdef __cplusplus
}
#endif

#endif /* INTERRUPTS_CONFIG_H_ */
