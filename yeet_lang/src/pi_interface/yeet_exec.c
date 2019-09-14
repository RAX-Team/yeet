//TODO DELETE FILE

/*#include <wiringPi.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <pthread.h>

#include "7seg.h"
#include "led.h"
#include "event_loop.h"
#include "../yeet.h"

#define USE_EVENT_LOOP true

void* event_loop_thread_function(void* arg) {
    Stack* stack = (Stack*) arg;
    start_event_loop(stack);
    return NULL;
}

int main(int arg_c, char** arg_v) {
    char* code = read_to_string(arg_v[1]);
    LexemeStream* stream = lex(code);
    free(code);
    Programme* programme = parse(stream);
    ls_cleanup(stream);
    Stack* result_stack = run(programme);
    if (USE_EVENT_LOOP) {
        pthread_t pthread_id;
        pthread_create(&pthread_id, NULL, event_loop_thread_function, result_stack);
        //start_event_loop(result_stack);
    }
    return EXIT_SUCCESS;
} */
