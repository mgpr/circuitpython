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
#include "py/binary.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/genclk/genclk.h"
#include "shared-bindings/util.h"

//| .. currentmodule:: genclk
//|
//| :class:`genclk` -- Generate clock signal into IO pin
//| ===================================================================
//|
//| Usage::
//|
//|    import genclk
//|    from board import *
//|
//|    clk1 = genclk.genclk(D1, 48)
//|    clk1.enable()
//|
//|    clk2 = genclk.genclk(D5, 4800)
//|    clk2.enable()
//|
//| .. class:: genclk(pin, freq_divisor)
//|
//|   Use the genclk on the given pin. The output frequency is the internal 48 MHz clock divided by the given divisor.
//|
//|   :param ~microcontroller.Pin pin: the pin to read from
//|
STATIC mp_obj_t genclk_genclk_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *pos_args) {
    // check number of arguments
    mp_arg_check_num(n_args, n_kw, 2, 2, false);
    
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);
    
    enum { ARG_CLOCKPIN, ARG_DIVISOR};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,      MP_ARG_REQUIRED | MP_ARG_OBJ},
        { MP_QSTR_divisor,  MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 1} },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    assert_pin(args[ARG_CLOCKPIN].u_obj, false);
    const mcu_pin_obj_t* pin = MP_OBJ_TO_PTR(args[ARG_CLOCKPIN].u_obj);
    assert_pin_free(pin);
    
    const uint32_t divisor = args[ARG_DIVISOR].u_int;
    
    genclk_genclk_obj_t *self = m_new_obj(genclk_genclk_obj_t);
    self->base.type = &genclk_genclk_type;
    
    common_hal_genclk_genclk_construct(self, pin, divisor);

    return (mp_obj_t) self;
}

//|   .. method:: deinit()
//|
//|      Deinitialises the genclk and releases any hardware resources for reuse.
//|
STATIC mp_obj_t genclk_genclk_deinit(mp_obj_t self_in) {
    genclk_genclk_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_genclk_genclk_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(genclk_genclk_deinit_obj, genclk_genclk_deinit);

//|   .. method:: __enter__()
//|
//|      No-op used by Context Managers.
//|
//  Provided by context manager helper.

//|   .. method:: __exit__()
//|
//|      Automatically deinitializes the hardware when exiting a context. See
//|      :ref:`lifetime-and-contextmanagers` for more info.
//|
STATIC mp_obj_t genclk_genclk_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_genclk_genclk_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(genclk_genclk___exit___obj, 4, 4, genclk_genclk_obj___exit__);


//|   .. method:: genclk.enable()
//|
//|      Enable clock output 
//|
//|
STATIC mp_obj_t genclk_genclk_enable(mp_obj_t self_in) {
    genclk_genclk_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_genclk_genclk_deinited(self));

    common_hal_genclk_genclk_enable(self);
    
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(genclk_genclk_enable_obj, genclk_genclk_enable);


//|   .. method:: genclk.disable()
//|
//|      Disable clock output 
//|
//|
STATIC mp_obj_t genclk_genclk_disable(mp_obj_t self_in) {
    genclk_genclk_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_genclk_genclk_deinited(self));

    common_hal_genclk_genclk_disable(self);
    
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(genclk_genclk_disable_obj, genclk_genclk_disable);


//|   .. attribute:: status
//|
//|     Whether the clock output is enabled. (read-only)
//|
//|
STATIC mp_obj_t genclk_genclk_obj_get_status(mp_obj_t self_in) {
    genclk_genclk_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_genclk_genclk_deinited(self));
    return mp_obj_new_bool(common_hal_genclk_genclk_get_status(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(genclk_genclk_get_status_obj, genclk_genclk_obj_get_status);

const mp_obj_property_t genclk_genclk_status_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&genclk_genclk_get_status_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};


STATIC const mp_rom_map_elem_t genclk_genclk_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&genclk_genclk___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&genclk_genclk_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&genclk_genclk_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&genclk_genclk_disable_obj) },
    
    { MP_OBJ_NEW_QSTR(MP_QSTR_status), MP_ROM_PTR(&genclk_genclk_status_obj)},
};

STATIC MP_DEFINE_CONST_DICT(genclk_genclk_locals_dict, genclk_genclk_locals_dict_table);

const mp_obj_type_t genclk_genclk_type = {
    { &mp_type_type },
    .name = MP_QSTR_genclk,
    .make_new = genclk_genclk_make_new,
    .locals_dict = (mp_obj_dict_t*)&genclk_genclk_locals_dict,
};
