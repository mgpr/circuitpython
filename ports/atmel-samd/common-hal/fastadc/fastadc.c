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

#include "common-hal/fastadc/fastadc.h"

#include <string.h>

#include "py/gc.h"
#include "py/nlr.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/mphal.h"

#include "samd/adc.h"
#include "shared-bindings/fastadc/fastadc.h"

#include "atmel_start_pins.h"
#include "hal/include/hal_adc_sync.h"
#include "hpl/gclk/hpl_gclk_base.h"

#include "samd/timers.h"

#ifdef SAMD21
#include "hpl/pm/hpl_pm_base.h"
#endif

void common_hal_fastadc_fastadc_construct(fastadc_fastadc_obj_t* self,
		const mcu_pin_obj_t *pin) {
	uint8_t fastadc_index;
	uint8_t fastadc_channel = 0xff;
	for (fastadc_index = 0; fastadc_index < NUM_ADC_PER_PIN; fastadc_index++) {
		// TODO(tannewt): Only use ADC0 on the SAMD51 when touch isn't being
		// used.
		if (pin->adc_input[fastadc_index] != 0xff) {
			fastadc_channel = pin->adc_input[fastadc_index];
			break;
		}
	}
	if (fastadc_channel == 0xff) {
		// No ADC function on that pin
		mp_raise_ValueError("Pin does not have ADC capabilities");
	}
	claim_pin(pin);

	gpio_set_pin_function(pin->pin, GPIO_PIN_FUNCTION_B);

	static Adc* adc_insts[] = ADC_INSTS;
	self->instance = adc_insts[fastadc_index];
	self->channel = fastadc_channel;
	self->pin = pin;
	self->prescaler = 3;
	self->samplen = 0;
	self->read_time = 0;
}

bool common_hal_fastadc_fastadc_deinited(fastadc_fastadc_obj_t *self) {
	return self->pin == mp_const_none;
}

void common_hal_fastadc_fastadc_deinit(fastadc_fastadc_obj_t *self) {
	if (common_hal_fastadc_fastadc_deinited(self)) {
		return;
	}
	reset_pin(self->pin->pin);
	self->pin = mp_const_none;
}

bool common_hal_fastadc_fastadc_get_values(fastadc_fastadc_obj_t* self, uint8_t *data, uint16_t nsamples){
	// Something else might have used the ADC in a different way,
	// so we completely re-initialize it.

	struct adc_sync_descriptor adc;
	
	// Init ADC. Assign ADC peripheral clock (clock 0 at 48MHz in SAMD21) and load calibration
	samd_peripherals_adc_setup(&adc, self->instance);

	// Full scale is 3.3V (VDDANA) = 65535.

	// On SAMD21, INTVCC1 is 0.5*VDDANA. On SAMD51, INTVCC1 is 1*VDDANA.
	// So on SAMD21 only, divide the input by 2, so full scale will match 0.5*VDDANA.
	adc_sync_set_reference(&adc, ADC_REFCTRL_REFSEL_INTVCC1_Val);
	#ifdef SAMD21
	adc_sync_set_channel_gain(&adc, self->channel, ADC_INPUTCTRL_GAIN_DIV2_Val);
	#endif

	//  Prescaler configuration
	// 0x3 - Peripheral clock divided by 32
	hri_adc_write_CTRLB_PRESCALER_bf(adc.device.hw, self->prescaler); // CLK_ADC = 48/32 = 1.5 MHz
	
	// Configure sample len [0-31]
	hri_adc_write_SAMPCTRL_SAMPLEN_bf(adc.device.hw, self->samplen);
	
	// SAMD21
	// samplen = 0
	// prescaler
	// 0          360.902 ksps  2.77083  us
	// 1          309.677 ksps  3.22917  us
	// 2          217.44  ksps  4.59896  us
	// 3          166.667 ksps  6.0  us
	// 4          93.75   ksps  10.6667  us
	// 5          53.5714 ksps  18.6667  us
	// 6          31.25   ksps  32.0  us
	
	// Set resolution
	adc_sync_set_resolution(&adc, ADC_CTRLB_RESSEL_8BIT_Val);

	// Set inputs
	adc_sync_set_inputs(&adc, self->channel, ADC_INPUTCTRL_MUXNEG_GND_Val, self->channel);

	// Enable ADC
	adc_sync_enable_channel(&adc, self->channel);

	// Setup counter to measure the number of cycles reading
	// which are saved in self->read_time
	bool tc_found = false;
	Tc* tc;
	uint8_t index;
	for (int8_t i = 0; i < TC_INST_NUM; i++){
		tc = tc_insts[i];
                if (hri_tc_get_CTRLA_ENABLE_bit(tc) == 0){
			tc_found = true;
			index = i;
			break;
                }
	}
	// Read twice and discard first result
	uint8_t value;
	adc_sync_read_channel(&adc, self->channel, &value, 1);
	
	if(tc_found){
		hri_tc_set_CTRLA_SWRST_bit(tc);
		while(hri_tc_get_CTRLA_SWRST_bit(tc));
		
		turn_on_clocks(true, index, 0); // 48 MHz clock
		
		hri_tc_write_CTRLA_reg(tc, TC_CTRLA_MODE_COUNT16 | 
					TC_CTRLA_WAVEGEN_NFRQ |
					TC_CTRLA_PRESCALER(0));
		
		hri_tc_set_CTRLA_ENABLE_bit(tc);
		hri_tc_wait_for_sync(tc);
	}
	bool ret;
	CRITICAL_SECTION_ENTER();
	ret = (adc_sync_read_channel(&adc, self->channel, data, nsamples) == nsamples);
	CRITICAL_SECTION_LEAVE();
	if(tc_found){
		self->read_time = hri_tccount16_read_COUNT_COUNT_bf(tc);
		hri_tc_clear_CTRLA_ENABLE_bit(tc);
	}
	
	adc_sync_deinit(&adc);
	return ret;
}

float common_hal_fastadc_fastadc_get_reference_voltage(fastadc_fastadc_obj_t *self) {
	return 3.3f;
}

uint16_t common_hal_fastadc_fastadc_get_read_time(fastadc_fastadc_obj_t* self)
{
	return self->read_time;
}

uint8_t common_hal_fastadc_fastadc_get_prescaler(fastadc_fastadc_obj_t* self)
{
	return self->prescaler;
}

void common_hal_fastadc_fastadc_set_prescaler(fastadc_fastadc_obj_t* self, uint8_t new_value)
{
	self->prescaler = new_value;
}

uint8_t common_hal_fastadc_fastadc_get_samplen(fastadc_fastadc_obj_t* self)
{
	return self->samplen;
}

void common_hal_fastadc_fastadc_set_samplen(fastadc_fastadc_obj_t* self, uint8_t new_value)
{
	self->samplen = new_value;
}
