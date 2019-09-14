#ifndef YEET_LANG_EVENT_LOOP_H
#define YEET_LANG_EVENT_LOOP_H

#include "../interpreter/stack.h"

#define RUNNING_STATE_FILE "/tmp/event_loop_running"

void start_event_loop(Stack* stack);

#endif // YEET_LANG_EVENT_LOOP_H
