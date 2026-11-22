//
// Created by Behin Mousavi on 14.11.26.
//

#include "matrix.h"
#include "quantum.h"
#include "print.h"

#include "keyboard.h"

// In keyboard.c
// extern analog_config he_config;

uint8_t analogReading[16];
// Starting  reference value of the sensors
uint8_t analogOffset[16];
// For returning the digital value from ADC to MCU. it's from 0 to 2^8-1 = 255
// GP5 is the most left digit
uint8_t analogReadPins[] = {GP5, GP6, GP7, GP8, GP9, GP10, GP11, GP12};

// DMUX pins, that are connected to MCU (Micro Controller Unit)
uint8_t swSelectPins[] = {GP13, GP14, GP15, GP16};

// DMUX's Chip Select so it's working
uint8_t enableSw0_15Pin = GP17;

// pins that are connected to ADC
// Start conversion from analog to digital in ADC
uint8_t convstPin_ = GP1;
// ADC Chip Select pin, when it's low means MCU wants to talk to ADC
uint8_t csPin_ = GP2;
// ADC Read pin, works with CS for MCU to talk to ADC
uint8_t rdPin_ = GP3;
// ADC is busy
uint8_t busyPin_ = GP4;

// currSW: switch number we want to read
uint8_t get_value(uint8_t currSW){
    uint8_t mask = 0b0001;
    // Write to DMUX which pin we want to know the value by setting the pins to it's binary
    // sizeof(swSelectPins) = memory size of whole array
    for (int i = 0; i < sizeof(key_select_pins)/sizeof(key_select_pins[0]); i++){
        // currSW & mask: Gets digits of currSW and set each pin
        gpio_write_pin(key_select_pins[i], currSW & mask);
        // Shift left to get next digit
        mask <<= 1;
    }

    // Give DMUX time to switch the pin and settle
    wait_ms(1);

    // Making falling edge on ADC conversion start sop ADC starts converting
    gpio_write_pin_low(convstPin_);     // conv start pin low
    gpio_write_pin_high(convstPin_);    // conv start pin back high

    // Wait for ADC to finish converting
    while(gpio_read_pin(busyPin_));     // wait till busy is low again
    // MCU wants talk to ADC
    gpio_write_pin_low(csPin_);         // CS low
    // Should be low with CS so MCU can talk
    gpio_write_pin_low(rdPin_);         // RD low

    uint8_t analogVal = 0;
    // Read the digital value from ADC digit by digit
    for (int i = 0; i < sizeof(digital_value_pins)/sizeof(digital_value_pins[0]); i++)
        analogVal += gpio_read_pin(digital_value_pins[i]) << i;

    // MCU stop the talk with ADC
    gpio_write_pin_high(rdPin_);         // RD high
    gpio_write_pin_high(csPin_);         // CS high

    return analogVal;
}

matrix_row_t previous_matrix[MATRIX_ROWS];

void matrix_init_custom(void) {
    // TODO: initialize hardware here

    for (int i = 0; i < sizeof(digital_value_pins)/sizeof(digital_value_pins[0]); i++)
        gpio_set_pin_input(digital_value_pins[i]);

    for (int i = 0; i < sizeof(key_select_pins)/sizeof(key_select_pins[0]); i++)
        gpio_set_pin_output(key_select_pins[i]);

    gpio_set_pin_output(enableSw0_15Pin);
    // DNUX always working
    gpio_write_pin_low(enableSw0_15Pin);


    gpio_set_pin_output(convstPin_);
    gpio_set_pin_output(csPin_);
    gpio_set_pin_output(rdPin_);
    gpio_set_pin_input(busyPin_);

    // Make initial conversion start bit high so later we have falling edge for it to convert the value
    gpio_write_pin_high(convstPin_);
    // Deactivate CS and RD
    gpio_write_pin_high(csPin_);
    gpio_write_pin_high(rdPin_);

    // Bad habit
    for (int currSW = 0; currSW < 16; currSW++){
        analogOffset[currSW] = get_value(currSW);
    }
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    // TODO: add matrix scanning routine here

    memcpy(previous_matrix, current_matrix, sizeof(previous_matrix));

    for (int currSW = 0; currSW < 16; currSW++){
        analogReading[currSW] = get_value(currSW);
        // actuation_point: the point the key should be activated
        if (analogReading[currSW] > analogOffset[currSW] + 38){
            uprintf("%d, %d\n", currSW, analogReading[currSW]);

            // Make a 4x4 grid
            int col = 0;
            int row = 0;
            if (currSW < 4){
                col = 0;
                row = currSW;
            }else if (currSW < 8){
                col = 1;
                row = currSW - 4;
            }else if (currSW < 12){
                col = 2;
                row = currSW - 8;
            }else if (currSW < 16){
                col = 3;
                row = currSW - 12;
            }

            // current_matrix is an array of rows. each row hase a binary Number that indicates each key's status. (Based on how Analog keyboards work.)
            current_matrix[row] |= (1 << col);
        }
        // Key is released
        else{

            int col = 0;
            int row = 0;
            if (currSW < 4){
                col = 0;
                row = currSW;
            }else if (currSW < 8){
                col = 1;
                row = currSW - 4;
            }else if (currSW < 12){
                col = 2;
                row = currSW - 8;
            }else if (currSW < 16){
                col = 3;
                row = currSW - 12;
            }

            current_matrix[row] &= ~(1 << col);
        }
    }

    //for (int i = 0; i < sizeof(swSelectPins)/sizeof(swSelectPins[0]); i++){
        //gpio_write_pin(swSelectPins[i], curr_sw & mask);
        //mask <<= 1;
    //}
    //gpio_write_pin_high(enableSw0_15Pin);
    //curr_sw++;

    //uint8_t analogReading = 0;
    //for (int i = 0; i < sizeof(analogReadPins)/sizeof(analogReadPins[0]); i++)
        //analogReading += gpio_read_pin(analogReadPins[i]) << i;

    //if (analogReading >= he_config.actuation_point){

    //}

    // If the state has changed
    return memcmp(previous_matrix, current_matrix, sizeof(previous_matrix)) != 0;
}