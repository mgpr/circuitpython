/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 M. Pereira
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>

#include "py/nlr.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "shared-bindings/genclk/genclk.h"

#include "hal/include/hal_gpio.h"

#include "samd/clocks.h"
#include "samd/pins.h"

void common_hal_genclk_genclk_construct(genclk_genclk_obj_t* self, const mcu_pin_obj_t *pin, const uint32_t divisor){

    uint8_t clock = pin->clock;
    if (clock == 0xff) {
        mp_raise_ValueError("Invalid pin");
    }
    
    if (gclk_enabled(clock)) {
        mp_raise_ValueError("Clock already in use");
    }
    
    if ((clock != 1 && divisor > 0xff) || (clock == 1 && divisor > 0xffff)) {
        mp_raise_ValueError("Too large divisor");
    }
    enable_clock_generator(clock, CLOCK_48MHZ, divisor);
    claim_pin(pin);
    
    self->pin = pin->pin;
    self->clock = clock;
    self->enabled = false;
}

bool common_hal_genclk_genclk_deinited(genclk_genclk_obj_t* self){
    return self->pin == NO_PIN;
}

void common_hal_genclk_genclk_deinit(genclk_genclk_obj_t* self){
    if (common_hal_genclk_genclk_deinited(self)) {
        return;
    }
    
    reset_pin(self->pin);
    self->enabled = false;
    self->pin = NO_PIN;
    disable_clock_generator(self->clock);
    reset_gclk(self->clock);
}

bool common_hal_genclk_genclk_get_status(genclk_genclk_obj_t *self){
    return self->enabled;
}

void common_hal_genclk_genclk_enable(genclk_genclk_obj_t* self){
    gpio_set_pin_direction(self->pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_pull_mode(self->pin, GPIO_PULL_OFF);
    gpio_set_pin_function(self->pin, MUX_H);
    self->enabled = true;
}

void common_hal_genclk_genclk_disable(genclk_genclk_obj_t* self){
    gpio_set_pin_function(self->pin, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(self->pin, GPIO_DIRECTION_OFF);
    self->enabled = false;
}

