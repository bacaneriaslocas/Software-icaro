#include "utilities.h"
#include "stm32h7xx_hal_gpio.h"

extern void SystemClock_Config(void);

/*
* DEFINICION DE MACROS PARA EL CONTROL DE LOS LEDS
*/
void led_test(void){
    CPU_P_ON();
    HAL_Delay(200);
    CPU_N_ON();
    HAL_Delay(200);
    SENS_P_ON();
    HAL_Delay(200);
    SENS_N_ON();
    HAL_Delay(200);
    BAT_P_ON();
    HAL_Delay(200);
    BAT_N_ON();
    HAL_Delay(200);
    RD_ON();
    HAL_Delay(200);
    GPS_ON();
    HAL_Delay(200);
    CPU_P_OFF();
    HAL_Delay(200);
    CPU_N_OFF();
    HAL_Delay(200);
    SENS_P_OFF();
    HAL_Delay(200);
    SENS_N_OFF();
    HAL_Delay(200);
    BAT_P_OFF();
    HAL_Delay(200);
    BAT_N_OFF();
    HAL_Delay(200);
    RD_OFF();
    HAL_Delay(200);
    GPS_OFF();
}
// funcion de control del zumbador
void Buzzer_on(uint32_t freq_hz){
    uint32_t timer_clk = 1000000; // 1 MHz tras prescaler
    uint32_t arr = (timer_clk / freq_hz) - 1;

    __HAL_TIM_SET_AUTORELOAD(&htim17, arr);
    __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, (arr + 1) / 2); // 50% duty

    HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
}
void Buzzer_off(void) { HAL_TIM_PWM_Stop(&htim17, TIM_CHANNEL_1); }
void uart_print(const char* s) { // control basico por UART
    HAL_UART_Transmit(&huart5, (uint8_t*)s, (uint16_t)strlen(s), HAL_MAX_DELAY);
}
void GPIO_Init(void){

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11 , GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 , GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOD,  GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOH,  GPIO_PIN_3 | GPIO_PIN_4, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOI,  GPIO_PIN_7, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_11, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);  

  GPIO_InitStruct.Pin = GPIO_PIN_12;                       
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct); // Pin de interrupcion del LoRa

  __HAL_RCC_SYSCFG_CLK_ENABLE();

  __HAL_RCC_GPIOE_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);  // BMI088 ACC INT1 (PE0)

  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);  // ICM42688P | LPS22HB | BMI088 GY (PI4)

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);

  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}
float getDinamicPressure(void){

	uint16_t raw;
	float volt;
	float Ps;

	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 20);
	raw = HAL_ADC_GetValue(&hadc2);

	volt = raw * RAW_TO_VOLTAGE;

	Ps = (volt - 3.285/2)/0.00125; // presión diferencial en pascales

	return Ps;
}
void Set_Servo_Angle(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t angle){
    // Limitar para evitar valores fuera de rango
    if (angle < 0.0f) angle = 0.0f;
    if (angle > 180.0f) angle = 180.0f;

    uint32_t pulse_length = 500 + (angle * (2000)/180);
    __HAL_TIM_SET_COMPARE(htim, channel, pulse_length);
}
void sys_init(bool vervosity) {
  
  sysinit_M4 = false;

  char msg[100];

  MPU_Config();
  HAL_Init();
  SystemClock_Config();

  GPIO_Init();
  MX_TIM17_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_UART5_Init();
  MX_I2C2_Init();
  MX_SPI2_Init();
  MX_ADC2_Init();

  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

  /*
 if (VL53L1__Init() != 0)
  {
    sprintf(msg, "** Error en la inicializacion del VL53L1_1 \r\n");
    uart_print(msg);
  } else {
    sprintf(msg, "** VL53L1_1 inicializado correctamente \r\n");
    uart_print(msg);
  }*/
  VL53L1X_StartRanging(VL53L1__ADDR); // comenzamos las mediciones continuas

  if(lps_init(&hspi2 ,GPIOE , GPIO_PIN_4)){
    sprintf(msg, "** LPS22HB inicializado correctamente \r\n" );
    uart_print(msg);
  } else {
    sprintf(msg, "** Error en la inicializacion del LPS22HB \r\n" );
    uart_print(msg);
  }
  if(icm_init(&hspi2, GPIOE, GPIO_PIN_5)){
    sprintf(msg, "** ICM42688P inicializado correctamente \r\n" );
    uart_print(msg);
  } else {
    sprintf(msg, "** Error en la inicializacion del ICM42688P \r\n" );
    uart_print(msg);
  }
  if(mmc_init(&hspi2, GPIOI, GPIO_PIN_7)){
    sprintf(msg, "** MMC5983MA inicializado correctamente \r\n" );
    uart_print(msg);
  } else {
    sprintf(msg, "** Error en la inicializacion del MMC5983MA \r\n" );
    uart_print(msg);
  }
  if(bmi_init(&hspi2, GPIOE, GPIO_PIN_3, GPIOE, GPIO_PIN_2)){
    sprintf(msg, "** BMI088 inicializado correctamente \r\n" );
    uart_print(msg);
  } else {
    sprintf(msg, "** Error en la inicializacion del BMI088 \r\n" );
    uart_print(msg);
  }

   
  HAL_RCCEx_EnableBootCore(RCC_GCR_BOOT_C2); // arrancamos el M4
  while (sysinit_M4 == false) {}// esperamos a que el M4 inicie sus perifericos

  sprintf(msg, "** System initialized \r\n" );
  uart_print(msg);

  Buzzer_on(1000);
  HAL_Delay(500);
  Buzzer_off();
  led_test();

  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);  // iniciamos la interrupcion del icm42688p
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);


}
void mat3_mul(float A[3][3],
                            float B[3][3],
                            float C[3][3]){
    // Opcional: usar una matriz temporal por si C apunta a A o B
    float T[3][3];

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            float sum = 0.0f;
            for (int k = 0; k < 3; k++) {
                sum += A[i][k] * B[k][j];
            }
            T[i][j] = sum;
        }
    }

    // Copiar el resultado
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            C[i][j] = T[i][j];
        }
    }
}
void mat3_transfer(float A[3][3], float C[3][3]) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      C[i][j] = A [i][j];
    }
  }
}