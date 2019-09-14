#include <wiringPi.h>

#include "io.h"
#include "../pi_interface/led.h"
#include "../pi_interface/7seg.h"

Data* turn_on(Data* device) {
    if (device->type == T_DEVICE) {
        const char* device_name = device->device.name;
        if (strncmp("Led", device_name, 3) != 0) {
            fprintf(stderr, "Cannot turn on non-led `%s`\n", device_name);
            exit(EXIT_FAILURE);
        }
        int led_id = device_name[strlen(device_name) - 1] - 'A';
        led_turn_on(leds[led_id]);
        return make_data_number(1);
    } else {
        fprintf(stderr, "Turn on only defined for devices\n");
        exit(EXIT_FAILURE);
    }
}

Data* turn_off(Data* device) {
    if (device->type == T_DEVICE) {
        const char* device_name = device->device.name;
        if (strncmp("Led", device_name, 3) != 0) {
            fprintf(stderr, "Cannot turn off non-led `%s`\n", device_name);
            exit(EXIT_FAILURE);
        }
        int led_id = device_name[strlen(device_name) - 1] - 'A';
        led_turn_off(leds[led_id]);
        return make_data_number(1);
    } else {
        fprintf(stderr, "Turn off defined for devices\n");
        exit(EXIT_FAILURE);
    }
}

Data* device_index(Data* device) {
    if (device->type == T_DEVICE) {
        const char* device_name = device->device.name;
        int device_id = device_name[strlen(device_name) - 1] - 'A';
        return make_data_number(device_id);
    } else {
        fprintf(stderr, "deviceIndex only defined for devices\n");
        exit(EXIT_FAILURE);
    }
}

Data* sleep(Data* length) {
    if (length->type == T_NUMBER) {
        delay((int) length->number.value);
        return make_data_number(0);
    } else {
        fprintf(stderr, "sleep only defined for numbers\n");
        exit(EXIT_FAILURE);
    }
}

Data* show_number(Data* device) {
    if (device->type == T_NUMBER) {
        show_number_7seg(device->number.value);
        return make_data_number(1);
    } else {
        fprintf(stderr, "showNumber only defined for numbers\n");
        exit(EXIT_FAILURE);
    }
}

Data* clear_number(void) {
    clear_7seg();
    return make_data_number(1);
}

Data* no_op(void) {
    return make_data_number(0);
}


Data* c_io_function(void* pointer, unsigned int argument_count) {
    Data* function = make_data_function(NULL);
    function->function.definitions[0].is_c = true;
    function->function.definitions[0].c_function = malloc(sizeof(CFunction));
    function->function.definitions[0].c_function->argument_count = argument_count;
    function->function.definitions[0].c_function->function = pointer;
    return function;
}

void import_io(Stack* stack) {
    add(stack, "turnOn", c_io_function(turn_on, 1));
    add(stack, "turnOff", c_io_function(turn_off, 1));
    add(stack, "whenOn", c_io_function(no_op, 1));
    add(stack, "whenOff", c_io_function(no_op, 1));
    add(stack, "showNumber", c_io_function(show_number, 1));
    add(stack, "screenOff", c_io_function(clear_number, 0));
    add(stack, "toggleOn", c_io_function(no_op, 1));
    add(stack, "toggleOff", c_io_function(no_op, 1));
    add(stack, "deviceNumber", c_io_function(device_index, 1));
    add(stack, "sleep", c_io_function(sleep, 1));
}
