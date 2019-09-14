#ifndef YEET_LANG_SWITCH_H
#define YEET_LANG_SWITCH_H

#include <stdbool.h>

#define NUM_SWITCHES 4

typedef enum SwitchPin {
    SWITCH1 = 21,
    SWITCH2 = 20,
    SWITCH3 = 26,
    SWITCH4 = 16
} SwitchPin;

typedef enum SwitchState {
    OFF, ON
} SwitchState;

typedef struct Switch {
    SwitchPin pin;
    SwitchState prev_state;
    SwitchState curr_state;
} Switch;

static const SwitchPin switch_pins[] = {SWITCH1, SWITCH2, SWITCH3, SWITCH4};
void init_switches(Switch* switches);
bool switch_is_on(Switch* hw_switch);
bool switch_toggled_on(Switch* hw_switch);
bool switch_toggled_off(Switch* hw_switch);
void update_switches_state(Switch* switches);

#endif //YEET_LANG_SWITCH_H
