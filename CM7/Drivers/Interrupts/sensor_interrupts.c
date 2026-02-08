/**
  ******************************************************************************
  * @file    sensor_interrupts.c
  * @brief   Implementation of sensor interrupt handlers
  ******************************************************************************
  * @attention
  * This module contains the actual interrupt service routines for sensors
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sensor_interrupts.h"
#include "stm32h7xx_hal_gpio.h"
#include "utilities.h"
#include "interrupts_config.h"

/* Sensor interrupt flags */
volatile bool icm_flag = false;      // ICM42688P interrupt flag
volatile bool bmigy_flag = false;    // BMI088_GY interrupt flag
volatile bool bmiacc_flag = false;   // BMI088_ACC interrupt flag
volatile bool vl53_flag = false;     // VL53L1X interrupt flag
volatile bool lps_flag = false;      // LPS22HBTR interrupt flag
volatile bool mmc_flag = false;      // MMC5983MA interrupt flag

/* Private function prototypes -----------------------------------------------*/
static void icm42688p_interrupt_handler(void);
static void bmi088_gy_interrupt_handler(void);
static void bmi088_acc_interrupt_handler(void);
static void mmc5983ma_interrupt_handler(void);
static void lps22hbtr_interrupt_handler(void);

/* Function Implementations --------------------------------------------------*/

/**
  * @brief EXTI0 interrupt handler (GPIO_PIN_0)
  */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);  // BMI088 ACC (INT1 -> PE0)
}

/**
  * @brief EXTI4 interrupt handler (GPIO_PIN_4)
  */
void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);  // BMI088 GY (INT3 -> PI4)
}

/**
  * @brief EXTI9_5 interrupt handler (GPIO_PIN_5, GPIO_PIN_6)
  * Rango EXTI5-9: GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8, GPIO_PIN_9
  * Note: HAL_GPIO_EXTI_IRQHandler() internally checks which pin triggered the interrupt
  *       and only calls HAL_GPIO_EXTI_Callback() for that specific pin
  */
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);  // LPS22HBTR
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);  // ICM42688P
}

/**
  * @brief Generic HAL GPIO EXTI callback
  * This is called by HAL_GPIO_EXTI_IRQHandler() for each GPIO pin
  * @param GPIO_Pin The GPIO pin that triggered the interrupt
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin) {
      case GPIO_PIN_0:
        // BMI088 ACC interrupt (INT1 -> PE0)
        bmi088_acc_interrupt_handler();
        break;

      case GPIO_PIN_4:
        // BMI088 GY interrupt (INT3 -> PI4)
        bmi088_gy_interrupt_handler();
        break;

      case GPIO_PIN_5:
        // LPS22HBTR interrupt
        lps22hbtr_interrupt_handler();
        break;

      case GPIO_PIN_6:
        // ICM42688P interrupt
        icm42688p_interrupt_handler();
        break;

      default:
        break;
    }
}

/**
  * @brief Handle ICM42688P sensor interrupt
  */
static void icm42688p_interrupt_handler(void) { icm_flag = true; }

/**
  * @brief Handle BMI088 sensor interrupt
  */
static void bmi088_gy_interrupt_handler(void) { bmigy_flag = true; }
  
static void bmi088_acc_interrupt_handler(void) { bmiacc_flag = true; }

/**
  * @brief Handle MMC5983MA sensor interrupt
  */
static void mmc5983ma_interrupt_handler(void) { mmc_flag = true; }

/**
 * @brief Handle LPS22HBTR sensor interrupt
 */
static void lps22hbtr_interrupt_handler(void) { lps_flag = true; }
