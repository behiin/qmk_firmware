//
// Created by Behin Mousavi on 16.11.26.
//

#pragma once

#include "quantum.h"

// ADC pins connected to MCU
/* ADC's Conversion Start pin (active-low)
 * When activated, starts conversion from analog to digital on ADC */
#define ADC_CONVSTn GP1
/* ADC's Chip Select pin (active-low)
 * When activated, MCU wants to talk to ADC */
#define ADC_CSn GP2
/* ADC's Read pin (active-low)
 * Works with CS pin for MCU to talk to ADC */
#define ADC_RDn GP3
/* ADC's Busy pin
 * Goes high during the conversion */
#define ADC_BUSY GP4
/* ADC's 8-bit digital output pins
 * MSB = GP5
 * Used by the MCU to read the key's analog value (0-255) */
#define ADC_READ_PINS { GP5, GP6, GP7, GP8, GP9, GP10, GP11, GP12 }

// DMUX pins connected to MCU
/* DMUX's Chip Select pin (active-low)
 * For it to start working */
#define DMUX_CSn GP17
/* DMUX's 4-bit key selection input pins
 * Used to select one of 16 keys (2^4) for the ADC to read */
#define DMUX_WRITE_PINS { GP13, GP14, GP15, GP16 }

// extern bool check;

extern const pin_t digital_value_pins[];
extern const size_t digital_value_pins_count;

extern const pin_t key_select_pins[];
extern const size_t key_select_pins_count;

void pins_assign(void);
void pins_init(void);
