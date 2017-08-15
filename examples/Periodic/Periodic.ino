/**
 * This example does a periodic brightness variation on the built in led.
 */

#define MAX_INTERVAL_US 50000 // 50 ms
#define PERIOD_MS 5000

#include <led_dither.h>

DitherLed led(LED_BUILTIN);

int get_brightness() {
    float v = (millis() % PERIOD_MS) / (float) PERIOD_MS;
    v = sin(M_PI * v);
    v = v*v;
    v *= DITHER_ON_VALUE;
    return (int)v;
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
