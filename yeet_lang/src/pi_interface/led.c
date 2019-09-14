#include <wiringPi.h>

#include "led.h"

void init_leds(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
        pinMode(leds[i], OUTPUT);
        led_turn_off(leds[i]);
    }
}

void led_turn_on(Led led) {
    digitalWrite(led, HIGH);
}

void led_turn_off(Led led) {
    digitalWrite(led, LOW);
}

bool led_is_on(Led led) {
    return digitalRead(led);
}
