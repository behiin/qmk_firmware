//
// Created by Behin Mousavi on 16.11.26.
//

#include "pins.h"
#include "gpio.h"

#include <stdbool.h>

bool check = false;
const pin_t digital_value_pins[] = ADC_READ_PINS;
const size_t digital_value_pins_count = sizeof(digital_value_pins) / sizeof(digital_value_pins[0]);

const pin_t key_select_pins[] = DMUX_WRITE_PINS;
const size_t key_select_pins_count = sizeof(key_select_pins) / sizeof(key_select_pins[0]);

void pins_assign(void) {
    for (uint8_t i = 0; i < digital_value_pins_count; i++)
        gpio_set_pin_input(digital_value_pins[i]);

    gpio_set_pin_output(ADC_CONVSTn);
    gpio_set_pin_output(ADC_CSn);
    gpio_set_pin_output(ADC_RDn);
    gpio_set_pin_input(ADC_BUSY);

    for (uint8_t i = 0; i < key_select_pins_count; i++)
        gpio_set_pin_output(key_select_pins[i]);

    gpio_set_pin_output(DMUX_CSn);
}

void pins_init(void) {
    gpio_write_pin_low(DMUX_CSn); // Activate DMUX, always working
    gpio_write_pin_high(ADC_CONVSTn);  // Initialise high, as later activates with falling edge
    gpio_write_pin_high(ADC_CSn); // Deactivate CS
    gpio_write_pin_high(ADC_RDn); // Deactivate RD
}