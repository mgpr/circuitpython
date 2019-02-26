/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_FASTADC_FASTADC_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_FASTADC_FASTADC_H

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/fastadc/fastadc.h"

extern const mp_obj_type_t fastadc_fastadc_type;

void common_hal_fastadc_fastadc_construct(fastadc_fastadc_obj_t* self, const mcu_pin_obj_t *pin);
void common_hal_fastadc_fastadc_deinit(fastadc_fastadc_obj_t* self);
bool common_hal_fastadc_fastadc_deinited(fastadc_fastadc_obj_t* self);

bool common_hal_fastadc_fastadc_get_values(fastadc_fastadc_obj_t* self, uint8_t *data, uint16_t len);

float common_hal_fastadc_fastadc_get_reference_voltage(fastadc_fastadc_obj_t* self);

uint16_t common_hal_fastadc_fastadc_get_read_time(fastadc_fastadc_obj_t* self);

uint8_t common_hal_fastadc_fastadc_get_prescaler(fastadc_fastadc_obj_t* self);
void common_hal_fastadc_fastadc_set_prescaler(fastadc_fastadc_obj_t* self, uint8_t new_value);

uint8_t common_hal_fastadc_fastadc_get_samplen(fastadc_fastadc_obj_t* self);
void common_hal_fastadc_fastadc_set_samplen(fastadc_fastadc_obj_t* self, uint8_t new_value);


#endif  // __MICROPY_INCLUDED_SHARED_BINDINGS_FASTADC_FASTADC_H__
