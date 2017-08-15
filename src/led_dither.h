#include <inttypes.h>

#define DITHER_OFF_VALUE 0
#define DITHER_ON_VALUE 255

class DitherLed {
    int pin;

    int target_value;
    int actual_value;

    int32_t updated_at;
    int32_t error;

    int32_t min_time_on;
    int32_t min_time_off;
    int32_t max_time_on;
    int32_t max_time_off;
    int32_t min_error;
    int32_t max_error;

private:
    int32_t update_brightness();
    void update_error();

public:
    DitherLed(int pin);

	/** Initializes the led pin as OUTPUT and sets brightness to 0 */
    void begin();

	/** Reset led pin to INPUT */
    void end();

	/**
	 * Updates the led ON/OFF state to achieve the desired average brightness.
	 *
	 * Returns how long you should wait (in micros before updating again.
	 * Calling update() before the elapsed time finishes is a no-op, and
	 * calling it late is also ok, as it can be compensated for later.
	 *
	 * If brightness is either 0 on MAX, it returns -1 and you won't need to update() until the brightness has changed.
	 */
    int32_t update();

	/**
	 * Set the new desired brightness and updates the led ON/OFF state.
	 *
	 * The same as `set_brightness(new_value) + update()`
	 */
    int32_t update(int new_value);

	/** Returns the current brightness */
	int get_brightness();

	/** Sets the new brightness, but doen't modify the LED state */
	void set_brightness(int new_value);
};
