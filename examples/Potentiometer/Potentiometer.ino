/**
 * This example reads the value of a potentiometer on A0 and uses
 * it to adjust the brightness of the built in led.
 */

#define MAX_INTERVAL_US 50000 // 50 ms

#include <led_dither.h>

DitherLed led(LED_BUILTIN);

int get_brightness() {
    int value = analogRead(A0);
    return map(value, 0, 1023, 0, DITHER_ON_VALUE);
}

void setup() {
    led.begin();
}

void loop() {
    int value = get_brightness();

    int wait = led.update(value);

    // We cannot delay forever, otherwise the program becomes irresponsive.
    // If the wait is either too big (wait > MAX_INTERVAL) or inifinite (wait < 0),
    // we wait only MAX_INTERVAL
    if (wait < 0 || wait > MAX_INTERVAL_US) {
        wait = MAX_INTERVAL_US;
    }
    delayMicroseconds(max(1,wait));
}
