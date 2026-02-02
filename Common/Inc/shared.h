#ifndef SHARED_H
#define SHARED_H

#include <stdbool.h>
#include <stdint.h>

extern volatile bool sysinit_M4;
extern volatile bool LoRa_state;
extern volatile bool sd_state;

extern volatile float actitud[3][3];
extern volatile float posicion[3];

extern volatile uint8_t len;
extern volatile uint8_t res;

extern volatile uint8_t buffer[128];
#endif