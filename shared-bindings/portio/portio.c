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
#include <stdio.h>
#include <string.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/portio/portio.h"
#include "shared-bindings/portio/port.h"
#include "shared-bindings/util.h"

//| .. currentmodule:: portio
//|
//| :class:`portio` -- Read simultaneously multiple IO pins
//| ===================================================================
//|
//|	Can be used to read multiple IO pins simultaneously. Optionally, the result can be masked and right shifted.
//|
//|
//| Usage::
//|
//|	import portio
//|
//|	port_masked = portio.portio(portio.Port.PORT_A, mask=0xF0, shift=4)
//|
//|	print(bin(port_masked.read_port()))
//|
//|
//| .. class:: portio(port, mask = 0xFFFFFFFF, shift = 0)
//|
//|
//|   :param ~portio.Port port: the port to read from
//|
STATIC mp_obj_t portio_portio_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *pos_args) {
	// check number of arguments
	mp_arg_check_num(n_args, n_kw, 1, 3, true);

	mp_map_t kw_args;
	mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);
	
	enum { ARG_PORT, ARG_MASK, ARG_SHIFT };
	static const mp_arg_t allowed_args[] = {
		{ MP_QSTR_Port,	 MP_ARG_REQUIRED | MP_ARG_OBJ},
		{ MP_QSTR_mask, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xFFFFFFFF} },
		{ MP_QSTR_shift, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
	};
	mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
	mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);	
	
	if (args[ARG_PORT].u_rom_obj != &portio_port_porta_obj && args[ARG_PORT].u_rom_obj != &portio_port_portb_obj){
		mp_raise_ValueError("Invalid port.");
	}

	portio_portio_obj_t *self = m_new_obj(portio_portio_obj_t);
	self->base.type = &portio_portio_type;

	if (args[ARG_PORT].u_rom_obj == &portio_port_porta_obj){
		common_hal_portio_portio_construct(self, PORT_A, args[ARG_MASK].u_int, args[ARG_SHIFT].u_int);
	}
	else{
		common_hal_portio_portio_construct(self, PORT_B, args[ARG_MASK].u_int, args[ARG_SHIFT].u_int);
	}

	return (mp_obj_t) self;
}

//|   .. method:: deinit()
//|
//|	  Deinitialises the portio and releases any hardware resources for reuse.
//|
STATIC mp_obj_t portio_portio_deinit(mp_obj_t self_in) {
	portio_portio_obj_t *self = MP_OBJ_TO_PTR(self_in);
	common_hal_portio_portio_deinit(self);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(portio_portio_deinit_obj, portio_portio_deinit);

//|   .. method:: __enter__()
//|
//|	  No-op used by Context Managers.
//|
//  Provided by context manager helper.

//|   .. method:: __exit__()
//|
//|	  Automatically deinitializes the hardware when exiting a context. See
//|	  :ref:`lifetime-and-contextmanagers` for more info.
//|
STATIC mp_obj_t portio_portio_obj___exit__(size_t n_args, const mp_obj_t *args) {
	(void)n_args;
	common_hal_portio_portio_deinit(args[0]);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(portio_portio___exit___obj, 4, 4, portio_portio_obj___exit__);


//|   .. method::  read_port()
//|
//|	  Reads port
//|
//|
STATIC mp_obj_t portio_portio_read_port(mp_obj_t self_in) {
	portio_portio_obj_t *self = MP_OBJ_TO_PTR(self_in);
	raise_error_if_deinited(common_hal_portio_portio_deinited(self));
	return MP_OBJ_NEW_SMALL_INT(common_hal_portio_portio_read_port(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(portio_portio_read_port_obj, portio_portio_read_port);


STATIC const mp_rom_map_elem_t portio_portio_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
	{ MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&portio_portio___exit___obj) },
	{ MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&portio_portio_deinit_obj) },

	{ MP_ROM_QSTR(MP_QSTR_read_port), MP_ROM_PTR(&portio_portio_read_port_obj) },
	
};

STATIC MP_DEFINE_CONST_DICT(portio_portio_locals_dict, portio_portio_locals_dict_table);

const mp_obj_type_t portio_portio_type = {
	{ &mp_type_type },
	.name = MP_QSTR_portio,
	.make_new = portio_portio_make_new,
	.locals_dict = (mp_obj_dict_t*)&portio_portio_locals_dict,
};
