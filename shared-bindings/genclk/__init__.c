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

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/genclk/__init__.h"
#include "shared-bindings/genclk/genclk.h"

#include "py/runtime.h"

//| :mod:`genclk` --- Generate output clock on pin
//| =================================================
//|
//| .. module:: genclk
//|   :synopsis: Hardware accelerated behavior
//|   :platform: SAMD21
//|
//| The `genclk` module contains classes to generate a clock signal into 
//| an output pin.
//|
//| Libraries
//|
//| .. toctree::
//|     :maxdepth: 3
//|
//|     genclk
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|    import genclk
//|    from board import *
//|   
//|    #  1 MHz clock signal at pin D1
//|    clk1 = genclk.genclk(D1, 48)
//|    clk1.enable()
//|
//|    #  10 kHz signal at pin D5
//|    clk2 = genclk.genclk(D5, 4800)
//|    clk2.enable()
//|
//|     Clock  IO Pin       Max Divisor
//|      0*	D2	     0xff
//|      1	D5	     0xffff
//|      2*	D11	     0xff
//|      3*	D13	     0xff
//|      4	D1, D6	     0xff
//|      5	D0, D7	     0xff
//|
//|	*Clocks 0, 2 and 3 are internally used so they are not available.
//|
//| This example will initialize the the device, and print the
//| :py:data:`~genclk.genclk.value`.
//|

STATIC const mp_rom_map_elem_t genclk_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_genclk) },
    { MP_ROM_QSTR(MP_QSTR_genclk),   MP_ROM_PTR(&genclk_genclk_type) },
};

STATIC MP_DEFINE_CONST_DICT(genclk_module_globals, genclk_module_globals_table);

const mp_obj_module_t genclk_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&genclk_module_globals,
};
