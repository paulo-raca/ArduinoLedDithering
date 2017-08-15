#include "led_dither.h"
#include <Arduino.h>

#define DITHER_MIN_PERIOD_MS 10
#define DITHER_MAX_PERIOD_MS 1000

DitherLed::DitherLed(int pin)
: pin(pin) {
}


 void DitherLed::begin() {
	pinMode(pin, OUTPUT);
	updated_at = micros();
	target_value = 0;
	actual_value = 0;
	error = 0;
	this->update(0);
}


void DitherLed::end() {
	pinMode(pin, INPUT);
}


void DitherLed::update_error() {
	int32_t now = micros();
	int32_t elapsed = (now - this->updated_at);
	this->updated_at = now;

	auto old_error = this->error;
	this->error += elapsed * (this->actual_value - this->target_value);

	if (this->error < this->min_error) {
		this->error = this->min_error;
	} else if (this->error > this->max_error) {
		this->error = this->max_error;
	}
}


int32_t DitherLed::update_brightness() {
	int32_t error_speed;
	int32_t timeout;

	if ((this->error < 0) || ( (this->error == 0) && (this->target_value > DITHER_ON_VALUE / 2) ) ) {
		digitalWrite(pin, HIGH);
		this->actual_value = DITHER_ON_VALUE;
// 		Serial.println("ON");
	} else {
		digitalWrite(pin, LOW);
		this->actual_value = DITHER_OFF_VALUE;
// 		Serial.println("OFF");
	}

	//Calculate how long it takes to cross error=0, and schedules the timer accordingly
	error_speed = this->target_value - this->actual_value;
	if (error_speed == 0) {
		// We're stable, no need to reschedule the timer until the value has changed.
		return -1;
	} else {
		timeout = this->error / error_speed;

		if (this->actual_value && (this->min_time_on >= this->min_time_off) && (timeout < this->min_time_on)) {
			timeout = this->min_time_on;
		}
		if (!this->actual_value && (this->min_time_off >= this->min_time_on) && (timeout < this->min_time_off)) {
			timeout = this->min_time_off;
		}
		return timeout;
	}
}


int32_t DitherLed::update() {
	this->update_error();
	return this->update_brightness();
}


int32_t DitherLed::update(int new_value) {
	this->set_brightness(new_value);
	return this->update_brightness();
}

void DitherLed::set_brightness(int new_value) {
	this->min_time_on  = (int32_t)1000 * DITHER_MIN_PERIOD_MS * (new_value - DITHER_OFF_VALUE) / (DITHER_ON_VALUE - DITHER_OFF_VALUE);
	this->min_time_off = (int32_t)1000 * DITHER_MIN_PERIOD_MS * (DITHER_ON_VALUE - new_value)  / (DITHER_ON_VALUE - DITHER_OFF_VALUE);

	this->max_time_on  = (int32_t)1000 * DITHER_MAX_PERIOD_MS * (new_value - DITHER_OFF_VALUE) / (DITHER_ON_VALUE - DITHER_OFF_VALUE);
	this->max_time_off = (int32_t)1000 * DITHER_MAX_PERIOD_MS * (DITHER_ON_VALUE - new_value)  / (DITHER_ON_VALUE - DITHER_OFF_VALUE);
	this->max_error = this->max_time_on  * (DITHER_ON_VALUE - new_value);
	this->min_error = this->max_time_off * (DITHER_OFF_VALUE - new_value);

	this->update_error();
	this->target_value = new_value;
}

int DitherLed::get_brightness() {
	return this->target_value;
}
