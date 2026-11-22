// TODO:Software Boot loader
// https://docs.qmk.fm/quantum_keycodes#qmk-keycodes
// QK_BOOTLOADER	QK_BOOT	Put the keyboard into bootloader mode for flashin
//
// https://docs.qmk.fm/platformdev_rp2040#gpio
// #define RP2040_BOOTLOADER_DOUBLE_TAP_RESET // Activates the double-tap behavior
// #define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 200U // Timeout window in ms in which the double tap can occur.
// #define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP17 // Specify a optional status led by GPIO number which blinks when entering the bootloader
//



// // //
// // Created by Behin Mousavi on 14.11.26.
// //
//
// #include "matrix.h"
// #include "print.h"
//
// matrix_row_t matrix_get_row(uint8_t row) {
//     // TODO: return the requested row data
// }
//
// void matrix_print(void) {
//     // TODO: use print() to dump the current matrix state to console
// }
//
// // Initialize starting value of keys
// // void set_keys_offeset(void) {
// //     // TODO: Statistics?
// //     for (uint8_t currSW = 0; currSW < 16; currSW++){
// //         analogOffset[currSW] = read_sensor(currSW);
// //     }
// // }
//
// void matrix_init(void) {
//     // TODO: initialize hardware and global matrix state here
//
//     // matrix_init_custom();
//
//     // set_keys_offeset();
//
//     // Initialize matrix state: all keys off
//     for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
//         raw_matrix[i] = 0;
//         matrix[i]     = 0;
//     }
//
//     // Unless hardware debouncing - Init the configured debounce routine
//     debounce_init();
//
//     // This *must* be called for correct keyboard behavior
//     matrix_init_kb();
// }
//
// uint8_t matrix_scan(void) {
//     bool changed = false;
//
//     // TODO: add matrix scanning routine here
//
//     // Unless hardware debouncing - use the configured debounce routine
//     changed = debounce(raw_matrix, matrix, changed);
//
//     // This *must* be called for correct keyboard behavior
//     matrix_scan_kb();
//
//     return changed;
// }

//####################################################
#include "matrix.h"
#include "print.h"
#include "pins.h"

uint8_t analogReading[16];
// Starting reference value of the sensors
uint8_t analogOffset[16];
// bool check = false;

bool changed = false;

// Write to DMUX
void select_key(uint8_t currSW) {
    gpio_write_pin(key_select_pins[0], currSW & 0b0001);
    gpio_write_pin(key_select_pins[1], currSW & 0b0010);
    gpio_write_pin(key_select_pins[2], currSW & 0b0100);
    gpio_write_pin(key_select_pins[3], currSW & 0b1000);
}

// currSW: switch number we want to read
uint8_t get_value(uint8_t key) {
    // Making falling edge on ADC conversion start pin so ADC starts converting
    gpio_write_pin_low(ADC_CONVSTn);
    gpio_write_pin_high(ADC_CONVSTn);

    // Select next key
    select_key((key + 1) % 16);

    // TODO: Use interupt instead of while
    // Wait for ADC to finish converting
    while(gpio_read_pin(ADC_BUSY));

    // MCU wants to talk to ADC
    gpio_write_pin_low(ADC_CSn);
    // Should be low with ADC's CS so MCU can talk to it
    gpio_write_pin_low(ADC_RDn);

    uint8_t analogVal = 0;
    // Read the digital value from ADC digit by digit
    for (int i = 0; i < digital_value_pins_count; i++)
        analogVal += gpio_read_pin(digital_value_pins[i]) << i;

    // MCU stop the talk with ADC
    gpio_write_pin_high(ADC_RDn);
    gpio_write_pin_high(ADC_CSn);

    return analogVal;
}

// Initialize starting value of keys
void set_offset(void) {
    // TODO: Statistics?
    for (uint8_t currSW = 0; currSW < 16; currSW++){
        analogOffset[currSW] = get_value(currSW);
    }
}

void matrix_init_custom(void) {
    // Select first key
    select_key(0);
    // Wait untill DMUX settles
    wait_us(10);

    set_offset();
}

matrix_row_t previous_matrix[MATRIX_ROWS];
bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    changed = false;

    // uprintf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    // uprintf("In matrix_init_custom\n");
    // uprintf("!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    // TODO: add matrix scanning routine here

    // it does the memcpy in debounce in none.c because Debounce = 0 in rules.mk
    // memcpy(previous_matrix, current_matrix, sizeof(previous_matrix));

    for (int currSW = 0; currSW < 16; currSW++){
        analogReading[currSW] = get_value(currSW);
        if (currSW == 6) {
            uprintf("Key %d = %d\n", currSW, analogReading[currSW]);
        }

        // // TODO: ctuation_point: the point the key should be activated
        // if (analogReading[currSW] > analogOffset[currSW] + 38){
        //
        //     // uprintf("%d, %d\n", currSW, analogReading[currSW]);
        //
        //     // Make a 4x4 grid
        //     int col = 0;
        //     int row = 0;
        //     if (currSW < 4){
        //         col = 0;
        //         row = currSW;
        //     }else if (currSW < 8){
        //         col = 1;
        //         row = currSW - 4;
        //     }else if (currSW < 12){
        //         col = 2;
        //         row = currSW - 8;
        //     }else if (currSW < 16){
        //         col = 3;
        //         row = currSW - 12;
        //     }
        //
        //     // TODO:
        //     // if ((current_matrix[row] & (1 << col)) == 0) {
        //     //     matrix_print();
        //     //     changed = true;
        //     //
        //     //     // current_matrix is an array of rows. each row hase a binary Number that indicates each key's status. (Based on how Analog keyboards work.)
        //     current_matrix[row] |= (1 << col);
        //     // }
        //
        // }
        // // Key is released
        // else{
        //     int col = 0;
        //     int row = 0;
        //     if (currSW < 4){
        //         col = 0;
        //         row = currSW;
        //     }else if (currSW < 8){
        //         col = 1;
        //         row = currSW - 4;
        //     }else if (currSW < 12){
        //         col = 2;
        //         row = currSW - 8;
        //     }else if (currSW < 16){
        //         col = 3;
        //         row = currSW - 12;
        //     }
        //
        //
        //     // // TODO:
        //     // if ((current_matrix[row] & (1 << col)) == 1) {
        //     //     changed = true;
        //     //
        //     //
        //     current_matrix[row] &= ~(1 << col);
        // }


        // }
    }

    // If the state has changed
    // return memcmp(previous_matrix, current_matrix, sizeof(previous_matrix)) != 0;
    // TODO: return changed?
    return changed;
}
