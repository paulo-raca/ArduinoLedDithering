/**
 * This example uses DeepSleepScheduler to show how to delegate the blinking of multiple LEDs to a scheduler.
 * 
 * It also schedules 2 other tasks to asynchronously modify the led brightness, one based on a periodic variation and other reads a potentiometer on A0.
 */

#include <DeepSleepScheduler.h>
#include <led_dither.h>

#define LED_1 LED_BUILTIN
#define LED_2 1

class AsyncBlinkedr: public Runnable {
public:
    DitherLed led;

    AsyncBlinkedr(int led_pin)
      : led(led_pin)
    { }

    void begin() {
      led.begin();
      run();
    }

    void end() {
      scheduler.removeCallbacks(this);
      led.end();
    }

    virtual void run() {
        int delay = led.update();
        if (delay >= 0) {
            scheduler.scheduleDelayed(this, max(1, delay / 1000));
        }
    }

    void set_brightness(int value) {
        int delay = led.update(value);
        scheduler.removeCallbacks(this);
        if (delay >= 0) {
            scheduler.scheduleDelayed(this, max(1, delay / 1000));
        }
    }
};


class AsyncBrightnessChanger: public Runnable {
public:
    AsyncBlinkedr blinker;
    int refresh_ms;

    AsyncBrightnessChanger(int led_pin, int refresh_ms) 
      : blinker(led_pin),
        refresh_ms(refresh_ms)
    { }

    virtual int get_brightness() = 0;

    void begin() {
      blinker.begin();
      run();
    }

    void end() {
      scheduler.removeCallbacks(this);
      blinker.end();
    }

    virtual void run() {
        blinker.set_brightness(get_brightness());
        scheduler.scheduleDelayed(this, refresh_ms);
    }
};

class PeriodicBrightnessChanger: public AsyncBrightnessChanger {
public:
    int period_ms;

    PeriodicBrightnessChanger(int led_pin, int refresh_ms, int period_ms) 
      : AsyncBrightnessChanger(led_pin, refresh_ms),
        period_ms(period_ms)
    { }

    virtual int get_brightness() {
        float v = (millis() % period_ms) / (float) period_ms;
        v = sin(M_PI * v);
        v = v*v;
        v *= DITHER_ON_VALUE;
        return (int)v;
    }
};


class AnalogBrightnessChanger: public AsyncBrightnessChanger {
public:
    int analog_pin;

    AnalogBrightnessChanger(int led_pin, int refresh_ms, int analog_pin) 
      : AsyncBrightnessChanger(led_pin, refresh_ms),
        analog_pin(analog_pin)
    { }

    virtual int get_brightness() {
        int ret = analogRead(analog_pin);
        return map(ret, 0, 1023, 0, DITHER_ON_VALUE);
    }
};

//This led will alternate between ON and OFF every 5s
PeriodicBrightnessChanger led_periodic(LED_1, 100, 5000);

//This led is controlled by a pot on Analog input A0
AnalogBrightnessChanger led_analog(LED_2, 100, A0);

void setup() {
    led_periodic.begin();
    led_analog.begin();
}

void loop() {
    scheduler.execute();
}
