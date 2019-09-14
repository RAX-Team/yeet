#include <wiringPi.h>

#include "../interpreter/interpreter.h"
#include "event_loop.h"
#include "led.h"
#include "switch.h"
#include "7seg.h"

bool is_running(FILE* file) {
    fseek(file, 0, SEEK_SET);
    bool running;
    fread(&running, sizeof(bool), 1, file);
    return running;
}

void trigger(Stack* stack, char* function_name, char* device_name) {
    Data* func = find(stack, function_name);
    Data* arguments[] = {make_data_device(device_name)};
    exec_function(&func->function, 1, arguments);
    free(arguments[0]);
}

void start_event_loop(Stack* stack) {
    FILE* running_state_file = fopen(RUNNING_STATE_FILE, "w+");
    bool running = true;
    fwrite(&running, sizeof(bool), 1, running_state_file);
    wiringPiSetupGpio();
    init_leds();
    Switch switches[NUM_SWITCHES];
    init_switches(switches);
    init_7seg();
    while (is_running(running_state_file)) {
        update_switches_state(switches);
        char device_name[] = "Switch ";
        for (int i = 0; i < NUM_SWITCHES; i++) {
            device_name[6] = 'A' + i;
            Switch* hw_switch = &switches[i];
            if (switch_is_on(hw_switch)) {
                trigger(stack, "whenOn", device_name);
            } else {
                trigger(stack, "whenOff", device_name);
            }

            if (switch_toggled_on(hw_switch)) {
                trigger(stack, "toggleOn", device_name);
            } else if (switch_toggled_off(hw_switch)) {
                trigger(stack, "toggleOff", device_name);
            }
        }

    }
    destroy_7seg();
    fclose(running_state_file);
}
