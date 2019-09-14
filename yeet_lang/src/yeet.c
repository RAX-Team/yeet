#include <stdbool.h>

#include "yeet.h"
#include "pi_interface/event_loop.h"

#define USE_EVENT_LOOP false

char* read_to_string(char* path) {
    FILE* file = fopen(path, "rb");
    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = malloc(length + 2);
    fread(buffer, 1, length, file);
    fclose(file);
    buffer[length] = '\n';
    buffer[length + 1] = '\0';
    return buffer;
}


int main(int arg_c, char** arg_v) {
    if (arg_c != 2) {
        printf("A file is required for processing\n"
               "Exactly one argument must be provided, however %d given",
               arg_c - 1);
        exit(1);
    }
    char* code = read_to_string(arg_v[1]);
    LexemeStream* stream = lex(code);
    free(code);
    Programme* programme = parse(stream);
    ls_cleanup(stream);
    Stack* result_stack = run(programme);
    if (USE_EVENT_LOOP) {
        start_event_loop(result_stack);
    }
    return EXIT_SUCCESS;
}
