//
// Created by Behin Mousavi on 16.11.26.
//

#include "pins.h"

void keyboard_pre_init_kb(void) {
    pins_assign();
    pins_init();
}