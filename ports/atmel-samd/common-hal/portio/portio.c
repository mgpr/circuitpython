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
#include "stdio.h"

#include "py/nlr.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "shared-bindings/portio/portio.h"
#include "shared-bindings/portio/port.h"

#include "hal/include/hal_gpio.h"

#include "samd/clocks.h"
#include "samd/pins.h"

void common_hal_portio_portio_construct(portio_portio_obj_t* self, portio_port_t port, uint32_t mask, uint32_t shift)
{
	switch(port){
	case PORT_A:
		self->port = GPIO_PORTA;
		break;
	case PORT_B:
		self->port = GPIO_PORTB;
		break;
	default:
		self->port = NO_PIN;
		break;
	}
	self->mask = mask;
	self->shift = shift;
}

bool common_hal_portio_portio_deinited(portio_portio_obj_t* self)
{
	return self->port == NO_PIN;
}

void common_hal_portio_portio_deinit(portio_portio_obj_t* self)
{
	if (common_hal_portio_portio_deinited(self)){
		return;
	}
	
	self->port = NO_PIN;
}

uint32_t common_hal_portio_portio_read_port(portio_portio_obj_t* self)
{
	return (gpio_get_port_level(self->port) & self->mask) >> self->shift;
}
