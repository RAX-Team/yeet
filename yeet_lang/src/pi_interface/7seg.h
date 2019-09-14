#ifndef DRIVER_7SEG_H
#define DRIVER_7SEG_H

#include <stdbool.h>

#define NUMBER_SEGMENTS 7
#define NUMBER_DISPLAYS 2
#define DISPLAY_DELAY 5


typedef enum SegmentPin {
    SEG1A = 4,
    SEG1B = 17,
    SEG1C = 9,
    SEG1D = 24,
    SEG1E = 23,
    SEG1F = 2,
    SEG1G = 3,
    SEG2A = 22,
    SEG2B = 10,
    SEG2C = 11,
    SEG2D = 8,
    SEG2E = 25,
    SEG2F = 27,
    SEG2G = 7
} SegmentPin;

typedef struct Segment {
    SegmentPin pin;
    bool is_set_on;
} Segment;

typedef Segment Display[NUMBER_SEGMENTS];

static const SegmentPin pins_7seg[NUMBER_DISPLAYS][NUMBER_SEGMENTS] = {
        {SEG1A, SEG1B, SEG1C, SEG1D, SEG1E, SEG1F, SEG1G},
        {SEG2A, SEG2B, SEG2C, SEG2D, SEG2E, SEG2F, SEG2G}
};

void init_7seg(void);
void destroy_7seg(void);
void show_number_7seg(int number);
void clear_7seg(void);

#endif //DRIVER_7SEG_H
