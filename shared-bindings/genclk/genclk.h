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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_GENCLK_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_GENCLK_H

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/genclk/genclk.h"

extern const mp_obj_type_t genclk_genclk_type;

void common_hal_genclk_genclk_construct(genclk_genclk_obj_t* self, const mcu_pin_obj_t *pin, const uint32_t divisor);
void common_hal_genclk_genclk_deinit(genclk_genclk_obj_t* self);
bool common_hal_genclk_genclk_deinited(genclk_genclk_obj_t* self);

bool common_hal_genclk_genclk_get_status(genclk_genclk_obj_t *self);

void common_hal_genclk_genclk_enable(genclk_genclk_obj_t* self);
void common_hal_genclk_genclk_disable(genclk_genclk_obj_t* self);


#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_GENCLK_H
