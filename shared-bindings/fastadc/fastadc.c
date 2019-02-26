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

#include <string.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/nlr.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/fastadc/fastadc.h"
#include "shared-bindings/util.h"

//| .. currentmodule:: fastadc
//|
//| :class:`fastadc` -- Analog to digital conversions at variable sample rate
//| =============================================
//|
//|	fastadc can be used to perform a series of 8-bit ADC at variable sample rate (max 360 kbps).
//|
//| Usage::
//|
//|	import fastadc
//|	from board import *
//|
//|	buffer = bytearray(30)
//|	fadc = fastadc.fastadc(board.A5)
//|
//|	fadc.get_values(buffer)
//|	for i in range(len(buff)):
//|		print(buff[i])
//|
//|
//| .. class:: fastadc(pin)
//|
//|
//|	:param ~microcontroller.Pin pin: the pin to read from
//|
STATIC mp_obj_t fastadc_fastadc_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
	mp_arg_check_num(n_args, n_kw, 1, 1, false);

	mp_obj_t pin_obj = args[0];
	assert_pin(pin_obj, false);

	const mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(pin_obj);
	assert_pin_free(pin);
	
	fastadc_fastadc_obj_t *self = m_new_obj(fastadc_fastadc_obj_t);
	self->base.type = &fastadc_fastadc_type;
	
	common_hal_fastadc_fastadc_construct(self, pin);

	return (mp_obj_t) self;
}

//|	.. method:: deinit()
STATIC mp_obj_t fastadc_fastadc_deinit(mp_obj_t self_in) {
	fastadc_fastadc_obj_t *self = MP_OBJ_TO_PTR(self_in);
	common_hal_fastadc_fastadc_deinit(self);
	return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(fastadc_fastadc_deinit_obj, fastadc_fastadc_deinit);

//|	.. method:: __enter__()
//|
//|	  No-op used by Context Managers.
//|
//  Provided by context manager helper.

//|	.. method:: __exit__()
//|
//|	  Automatically deinitializes the hardware when exiting a context. See
//|	  :ref:`lifetime-and-contextmanagers` for more info.
//|
STATIC mp_obj_t fastadc_fastadc___exit__(size_t n_args, const mp_obj_t *args) {
	(void)n_args;
	common_hal_fastadc_fastadc_deinit(args[0]);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fastadc_fastadc___exit___obj, 4, 4, fastadc_fastadc___exit__);

//|	.. method:: fastadc.get_values(buffer)
//|
//|	Performs a number of sequential ADC. The total number of analog to digital conversions is determined from the buffer size.
//|
//|	buffer must be a bytearray.
//|
STATIC mp_obj_t fastadc_fastadc_obj_get_values(mp_obj_t self_in, mp_obj_t wr_buf) {
	fastadc_fastadc_obj_t *self = MP_OBJ_TO_PTR(self_in);
	raise_error_if_deinited(common_hal_fastadc_fastadc_deinited(self));
	
	mp_buffer_info_t bufinfo;
	mp_get_buffer_raise(wr_buf, &bufinfo, MP_BUFFER_WRITE);
	if (bufinfo.len == 0) {
		return mp_const_none;
	}
	
	if (bufinfo.typecode != BYTEARRAY_TYPECODE) {
		mp_raise_ValueError("buffer must be a bytearray");
        }

	bool ok = common_hal_fastadc_fastadc_get_values(self, bufinfo.buf, bufinfo.len);
	if(!ok){
		mp_raise_OSError(MP_EIO);
	}
	return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(fastadc_fastadc_get_values_obj, fastadc_fastadc_obj_get_values);

//|	.. attribute:: reference_voltage
//|
//|	 The maximum voltage measurable (also known as the reference voltage) as a
//|	 `float` in Volts.
//|
STATIC mp_obj_t fastadc_fastadc_obj_get_reference_voltage(mp_obj_t self_in) {
	fastadc_fastadc_obj_t *self = MP_OBJ_TO_PTR(self_in);
	raise_error_if_deinited(common_hal_fastadc_fastadc_deinited(self));
	return mp_obj_new_float(common_hal_fastadc_fastadc_get_reference_voltage(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(fastadc_fastadc_get_reference_voltage_obj, fastadc_fastadc_obj_get_reference_voltage);

const mp_obj_property_t fastadc_fastadc_reference_voltage_obj = {
	.base.type = &mp_type_property,
	.proxy = {(mp_obj_t)&fastadc_fastadc_get_reference_voltage_obj,
		(mp_obj_t)&mp_const_none_obj,
		(mp_obj_t)&mp_const_none_obj},
};

//|	.. attribute:: read_time
//|
//|	 Returns the number of microcontroller clock ticks spent during the last ADC acquisition triggered by the get_values method (read-only).
//|
//|	 `16-bit integer` clock ticks.
//|
STATIC mp_obj_t fastadc_fastadc_obj_get_read_time(mp_obj_t self_in) {
	fastadc_fastadc_obj_t *self = MP_OBJ_TO_PTR(self_in);
	raise_error_if_deinited(common_hal_fastadc_fastadc_deinited(self));
	return MP_OBJ_NEW_SMALL_INT(common_hal_fastadc_fastadc_get_read_time(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(fastadc_fastadc_obj_get_read_time_obj, fastadc_fastadc_obj_get_read_time);

const mp_obj_property_t fastadc_fastadc_read_time_obj = {
	.base.type = &mp_type_property,
	.proxy = {(mp_obj_t)&fastadc_fastadc_obj_get_read_time_obj,
		(mp_obj_t)&mp_const_none_obj,
		(mp_obj_t)&mp_const_none_obj},
};

//|	.. attribute:: clk_prescaler
//|
//|	 ADC clock prescaler. Define the ADC clock relative to the peripheral clock (48MHz in SAMD21).
//|
//|      0x0 - Peripheral clock divided by 4
//|      0x1 - Peripheral clock divided by 8
//|      0x2 - Peripheral clock divided by 16
//|      0x3 - Peripheral clock divided by 32
//|      0x4 - Peripheral clock divided by 64
//|      0x5 - Peripheral clock divided by 128
//|      0x6 - Peripheral clock divided by 256
//|      0x7 - Peripheral clock divided by 512
//|
STATIC mp_obj_t fastadc_fastadc_obj_get_prescaler(mp_obj_t self_in) {
	fastadc_fastadc_obj_t *self = MP_OBJ_TO_PTR(self_in);
	raise_error_if_deinited(common_hal_fastadc_fastadc_deinited(self));
	return MP_OBJ_NEW_SMALL_INT(common_hal_fastadc_fastadc_get_prescaler(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(fastadc_fastadc_get_prescaler_obj, fastadc_fastadc_obj_get_prescaler);

STATIC mp_obj_t fastadc_fastadc_obj_set_prescaler(mp_obj_t self_in, mp_obj_t arg_value) {
	fastadc_fastadc_obj_t *self = MP_OBJ_TO_PTR(self_in);
	raise_error_if_deinited(common_hal_fastadc_fastadc_deinited(self));
	
	uint8_t new_value = mp_obj_get_int(arg_value);
	
	if(new_value > 7){
		mp_raise_ValueError("Invalid prescaler.");
	}
	common_hal_fastadc_fastadc_set_prescaler(self, new_value);
	return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(fastadc_fastadc_set_prescaler_obj, fastadc_fastadc_obj_set_prescaler);


const mp_obj_property_t fastadc_fastadc_prescaler_obj = {
	.base.type = &mp_type_property,
	.proxy = {(mp_obj_t)&fastadc_fastadc_get_prescaler_obj,
		(mp_obj_t)&fastadc_fastadc_set_prescaler_obj,
		(mp_obj_t)&mp_const_none_obj},
};


//|	.. attribute:: samplen
//|
//|	 Sampling time length controlling the ADC input impedance. Valid values are 0 to 31
//|
//|	Sampling time = (samplen + 1)*(Clock_ADC/2)
//|
STATIC mp_obj_t fastadc_fastadc_obj_get_samplen(mp_obj_t self_in) {
	fastadc_fastadc_obj_t *self = MP_OBJ_TO_PTR(self_in);
	raise_error_if_deinited(common_hal_fastadc_fastadc_deinited(self));
	return MP_OBJ_NEW_SMALL_INT(common_hal_fastadc_fastadc_get_samplen(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(fastadc_fastadc_get_samplen_obj, fastadc_fastadc_obj_get_samplen);

STATIC mp_obj_t fastadc_fastadc_obj_set_samplen(mp_obj_t self_in, mp_obj_t arg_value) {
	fastadc_fastadc_obj_t *self = MP_OBJ_TO_PTR(self_in);
	raise_error_if_deinited(common_hal_fastadc_fastadc_deinited(self));

	uint8_t new_value = mp_obj_get_int(arg_value);
	
	if(new_value > 31){
		mp_raise_ValueError("Invalid samplen.");
	}
	common_hal_fastadc_fastadc_set_samplen(self, new_value);
	return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(fastadc_fastadc_set_samplen_obj, fastadc_fastadc_obj_set_samplen);


const mp_obj_property_t fastadc_fastadc_samplen_obj = {
	.base.type = &mp_type_property,
	.proxy = {(mp_obj_t)&fastadc_fastadc_get_samplen_obj,
		(mp_obj_t)&fastadc_fastadc_set_samplen_obj,
		(mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t fastadc_fastadc_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&fastadc_fastadc_deinit_obj) },
	{ MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
	{ MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&fastadc_fastadc___exit___obj) },
	
	{ MP_ROM_QSTR(MP_QSTR_get_values), MP_ROM_PTR(&fastadc_fastadc_get_values_obj)},
	
	{ MP_ROM_QSTR(MP_QSTR_reference_voltage), MP_ROM_PTR(&fastadc_fastadc_reference_voltage_obj)},
	{ MP_ROM_QSTR(MP_QSTR_read_time), MP_ROM_PTR(&fastadc_fastadc_read_time_obj)},
	{ MP_ROM_QSTR(MP_QSTR_prescaler), MP_ROM_PTR(&fastadc_fastadc_prescaler_obj)},
	{ MP_ROM_QSTR(MP_QSTR_samplen), MP_ROM_PTR(&fastadc_fastadc_samplen_obj)},
};

STATIC MP_DEFINE_CONST_DICT(fastadc_fastadc_locals_dict, fastadc_fastadc_locals_dict_table);

const mp_obj_type_t fastadc_fastadc_type = {
	{ &mp_type_type },
	.name = MP_QSTR_fastadc,
	.make_new = fastadc_fastadc_make_new,
	.locals_dict = (mp_obj_t)&fastadc_fastadc_locals_dict,
};
