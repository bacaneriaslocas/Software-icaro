#include "shared.h"

/* DEFINICIÓN: colocada en la sección .shared_ram */
__attribute__((section(".shared_ram"))) volatile bool sysinit_M4 = false;
__attribute__((section(".shared_ram"))) volatile bool LoRa_state = false;
__attribute__((section(".shared_ram"))) volatile bool sd_state = false;

__attribute__((section(".shared_ram"))) volatile uint8_t len = 0;
__attribute__((section(".shared_ram"))) volatile uint8_t res = 0;

__attribute__((section(".shared_ram"))) volatile uint8_t buffer[128] = {0};

__attribute__((section(".shared_ram"),
               aligned(32))) volatile float actitud[3][3] = {
    {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
__attribute__((section(".shared_ram"),  aligned(32))) volatile float posicion[3] = {0.0f, 0.0f, 0.0f};
