#include "main.h"
#include "dma.h"
#include "fatfs.h"
#include "spi.h"
#include "stm32h747xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"
#include "usart.h"
#include "gpio.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "shared.h"

#include "sx1276.h"
#include "sd_functions.h"

lora_sx1276 radio;
bool send = true;
uint32_t tmp = 0;

void uart_print(const char* s) { // control basico por UART
    HAL_UART_Transmit(&huart5, (uint8_t*)s, (uint16_t)strlen(s), HAL_MAX_DELAY);
}

float prev;

int main(void) {

  HAL_Init();
  MX_SPI3_Init();
  MX_SPI1_Init();

  if(sd_mount() == 0){              // inicializacion de la sd
    sd_state = true;
    sd_append_file("klk.txt", "Prueba de escritura \n");
  } else {
    sd_state = false;
  }

  sd_unmount();



  
  radio.spi = &hspi3;
  radio.nss_port = GPIOG;
  radio.nss_pin = GPIO_PIN_13;
  radio.frequency = 868ULL * 1000000ULL;
  radio.pa_mode = LORA_PA_OUTPUT_PA_BOOST;

  if (lora_init(&radio, radio.spi, radio.nss_port, radio.nss_pin,
                radio.frequency) == LORA_OK) {
    LoRa_state = true;
    lora_enable_interrupt_tx_done(&radio);
    lora_set_spreading_factor(&radio, 7);
    lora_set_signal_bandwidth(&radio, 9);
  } else {
      LoRa_state = false;
    }


    sysinit_M4 = true;



  while (1)
  {

    if (send) { // secuencia envio paquete
      lora_enable_interrupt_tx_done(&radio);
      char msg[124];

      float c00 = actitud[0][0], c01 = actitud[0][1], c02 = actitud[0][2];
      float c10 = actitud[1][0], c11 = actitud[1][1], c12 = actitud[1][2];
      float c20 = actitud[2][0], c21 = actitud[2][1], c22 = actitud[2][2];

      int n = snprintf(msg, sizeof(msg),
                      "%.4f,%.4f,%.4f; %.4f,%.4f,%.4f; %.4f,%.4f,%.4f\n", c00,
                      c10, c20, c01, c11, c21, c02, c12, c22);
      
      if ((size_t)n >= sizeof(msg)) {
          msg[sizeof(msg) - 2] = '\n';
          msg[sizeof(msg) - 1] = '\0';
          n = (int)strlen(msg);
      }
      lora_send_packet(&radio, (uint8_t *)msg, (uint8_t)n);
      send = false;
      tmp = HAL_GetTick();
    }

    if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_12) == GPIO_PIN_SET){ // habiendo terminado la transmision, se limpia Tx flag y se vuelve al modo recepcion continua
      lora_clear_interrupt_tx_done(&radio);

      lora_mode_receive_continuous(&radio);
      lora_enable_interrupt_rx_done(&radio);
    }
    if (lora_is_packet_available(&radio)) {
      len = lora_receive_packet(&radio, buffer, sizeof(buffer), &res);
      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_0);
    }   
               
    if((HAL_GetTick() - tmp) > 500){ // si pasan 0.5 segundos sin recibir nada, se vuelve a enviar
      send = true;
      lora_clear_interrupt_rx_all(&radio);
    }
  }
}

