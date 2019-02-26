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

#include <stdint.h>
#include <string.h>

#include "lib/utils/context_manager_helpers.h"

#include "py/nlr.h"
#include "py/objtype.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/portio/portio.h"

//| .. currentmodule:: portio
//|
//| :class:`Port` -- defines the microcontroller IO port
//| =============================================================
//|
//| .. class:: portio.Port
//|
//|     Enum-like class to define which port the digital values are
//|     going.
//|
//|     .. data:: PORT_A
//|
//|       Port A of the microcontroller
//|
//|     .. data:: PORT_B
//|
//|       Port B of the microcontroller
//|
const mp_obj_type_t portio_port_type;

const portio_port_obj_t portio_port_porta_obj = {
    { &portio_port_type },
};

const portio_port_obj_t portio_port_portb_obj = {
    { &portio_port_type },
};

STATIC const mp_rom_map_elem_t portio_port_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_PORT_A),   MP_ROM_PTR(&portio_port_porta_obj) },
    { MP_ROM_QSTR(MP_QSTR_PORT_B),   MP_ROM_PTR(&portio_port_portb_obj) },
};
STATIC MP_DEFINE_CONST_DICT(portio_port_locals_dict, portio_port_locals_dict_table);

STATIC void portio_port_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr port = MP_QSTR_PORT_A;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&portio_port_portb_obj)) {
        port = MP_QSTR_PORT_B;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_portio, MP_QSTR_Port, port);
}

const mp_obj_type_t portio_port_type = {
    { &mp_type_type },
    .name = MP_QSTR_Port,
    .print = portio_port_print,
    .locals_dict = (mp_obj_t)&portio_port_locals_dict,
};
