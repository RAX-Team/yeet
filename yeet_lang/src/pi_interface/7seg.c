#include <wiringPi.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>

#include "7seg.h"

static volatile bool multiplexing;
static pthread_t multiplexing_thread_id;
static Display* displays;

void display_turn_off(Display display) {
    for (int i = 0; i < NUMBER_SEGMENTS; i++) {
        display[i].is_set_on = false;
    }
}

void* multiplex_displays(void* arg) {
    int display_id = 0;
    while (multiplexing) {
        for (int i = 0; i < NUMBER_DISPLAYS; i++) {
            if (i == display_id) {
                for (int j = 0; j < NUMBER_SEGMENTS; j++) {
                    if (displays[i][j].is_set_on) {
                        digitalWrite(displays[i][j].pin, LOW);
                    } else {
                        digitalWrite(displays[i][j].pin, HIGH);
                    }
                }
            } else {
                for (int j = 0; j < NUMBER_SEGMENTS; j++) {
                    digitalWrite(displays[i][j].pin, HIGH);
                }
            }
            delay(DISPLAY_DELAY);
        }
        display_id = 1 - display_id;
    }
    return NULL;
}

void init_7seg(void) {
    displays = malloc(sizeof(Display) * NUMBER_DISPLAYS);
    for (int i = 0; i < NUMBER_DISPLAYS; i++) {
        for (int j = 0; j < NUMBER_SEGMENTS; j++) {
            displays[i][j].pin = pins_7seg[i][j];
            displays[i][j].is_set_on = false;
            pinMode(displays[i][j].pin, OUTPUT);
        }
    }
    multiplexing = true;
    pthread_create(&multiplexing_thread_id, NULL, multiplex_displays, NULL);
}

void destroy_7seg(void) {
    multiplexing = false;
    pthread_join(multiplexing_thread_id, NULL);
    free(displays);
}


void segment_turn_off(Segment* segment) {
    segment->is_set_on = false;
}

void segment_turn_on(Segment* segment) {
    segment->is_set_on = true;
}


void segments_turn_on_by_labels(Display display, char* labels) {
    int len = strlen(labels);
    for (int i = 0; i < len; i++) {
        segment_turn_on(&display[labels[i] - 'a']);
    }
}

void display_show_digit(Display display, int digit) {
	display_turn_off(display);
	switch (digit) {
        case 0:
            segments_turn_on_by_labels(display, "abcedf");
            break;
        case 1:
            segments_turn_on_by_labels(display, "bc");
            break;
	    case 2:
            segments_turn_on_by_labels(display, "abged");
            break;
	    case 3:
            segments_turn_on_by_labels(display, "abgcd");
            break;
        case 4:
            segments_turn_on_by_labels(display, "fgbc");
            break;
        case 5:
            segments_turn_on_by_labels(display, "afgcd");
            break;
        case 6:
            segments_turn_on_by_labels(display, "gcdefa");
            break;
        case 7:
            segments_turn_on_by_labels(display, "abc");
            break;
        case 8:
            segments_turn_on_by_labels(display, "abcdefg");
            break;
        case 9:
            segments_turn_on_by_labels(display, "gfabcd");
            break;
        default:
            fprintf(stderr, "%d is not a valid digit", digit);
            break;
	}
}

void clear_7seg(void) {
    display_turn_off(displays[0]);
    display_turn_off(displays[1]);
}

void show_number_7seg(int number) {
    clear_7seg();
    if (0 > number || number > 99) {
        fprintf(stderr, "%d is out of bounds, the display only supports numbers between 0 and 99 inclusive", number);
        return;
    }
    int digit0 = number / 10;
    int digit1 = number % 10;
    display_show_digit(displays[0], digit0);
    display_show_digit(displays[1], digit1);
}


