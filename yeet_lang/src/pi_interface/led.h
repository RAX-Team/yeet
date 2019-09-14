#ifndef YEET_LANG_LED_H
#define YEET_LANG_LED_H

#include <stdbool.h>

#define NUM_LEDS 8

typedef enum Led {
    LED1 = 14,
    LED2 = 15,
    LED3 = 18,
    LED4 = 5,
    LED5 = 6,
    LED6 = 12,
    LED7 = 13,
    LED8 = 19
} Led;

static const Led leds[] = {LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8};
void led_turn_on(Led led);
void led_turn_off(Led led);
void init_leds(void);
bool led_is_on(Led led);

#endif //YEET_LANG_LED_H
