#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>

#include "switch.h"

void init_switches(Switch* switches) {
    for (int i = 0; i < NUM_SWITCHES; i++) {
        SwitchPin pin = switch_pins[i];
        pinMode(pin, INPUT);
        SwitchState state = digitalRead(pin);
        switches[i] = (Switch) {.pin = pin,
                                .prev_state = state,
                                .curr_state = state};
    }
}

void update_switches_state(Switch* switches) {
    for (int i = 0; i < NUM_SWITCHES; i++) {
        Switch* hw_switch = &switches[i];
        hw_switch->prev_state = hw_switch->curr_state;
        hw_switch->curr_state = digitalRead(hw_switch->pin);
    }
}

bool switch_is_on(Switch* hw_switch) {
    return hw_switch->curr_state == ON;
}

bool switch_toggled_on(Switch* hw_switch) {
    return (hw_switch->prev_state == OFF) && (hw_switch->curr_state == ON);
}

bool switch_toggled_off(Switch* hw_switch) {
    return (hw_switch->prev_state == ON) && (hw_switch->curr_state == OFF);
}