//
// Created by matthewross on 15/06/19.
//

#include "prelude.h"

void rtrim(const char* set, char* text) {
    while (*text != '\0') {
        text++;
    }
    for (text--; strchr(set, *text); text--) {
        *text = '\0';
    }
}

void ltrim(const char* set, char* text) {
    while (strchr(set, *text)) {
        for (char *cursor = text; *cursor; cursor++)
            cursor[0] = cursor[1];
    }
}

Data* text_trim(Data* char_set, Data* text) {
    if (text->type != T_TEXT || char_set->type != T_TEXT) {
        fprintf(stderr, "Cannot trim non-string value\n");
        exit(EXIT_FAILURE);
    }
    char* string = strdup(text->text.value);
    rtrim(char_set->text.value, string);
    ltrim(char_set->text.value, string);
    make_data_text(string);
}

Data* say(Data* input) {
    DEBUGGING printf("Programme Output: ");
    print_data(input);
    puts("");
    return make_data_number(0); // TODO introduce `Nothing`?
}

Data* ask(Data* input) {
    print_data(input);
    printf("\n>> ");
    char* answer = calloc(101, sizeof(char)); // Probably should make dynamic
    fgets(answer, 100, stdin);
    rtrim("\n", answer);
    return make_data_text(answer);
}

Data* throw(Data* input) {
    fprintf(stderr, "Prelude/error! %s", (input->type == T_TEXT) ? input->text.value : "Error!");
    exit((input->type == T_NUMBER) ? ((int) input->number.value) : EXIT_FAILURE);
    return make_data_number(0);
}

Data* and(Data* left, Data* right) {
    return make_data_number(is_true(left) && is_true(right));
}

Data* or(Data* left, Data* right) {
    return make_data_number(is_true(left) || is_true(right));
}

Data* addition(Data* left, Data* right) {
    if (left->type != right->type) {
        fprintf(stderr, "Cannot add two values of different types\n");
        exit(EXIT_FAILURE);
    }
    int length;
    switch (left->type) {
        case T_NUMBER:
            return make_data_number(left->number.value + right->number.value);
        case T_TEXT:
            length = strlen(left->text.value) + strlen(right->text.value);
            char* concatenation = malloc(length + 1);
            concatenation[length] = '\0';
            return make_data_text(concatenation);
        default:
            fprintf(stderr, "Add only defined for numbers and strings\n");
            exit(EXIT_FAILURE);
    }
}

Data* subtract(Data* left, Data* right) {
    if (left->type != right->type) {
        fprintf(stderr, "Cannot subtract two values of different types\n");
        exit(EXIT_FAILURE);
    }
    switch (left->type) {
        case T_NUMBER:
            return make_data_number(left->number.value - right->number.value);
        default:
            fprintf(stderr, "Subtract only defined for numbers\n");
            exit(EXIT_FAILURE);
    }
}

Data* minus(Data* number) {
    switch (number->type) {
        case T_NUMBER:
            return make_data_number(-number->number.value);
        default:
            fprintf(stderr, "Minus only defined for numbers\n");
            exit(EXIT_FAILURE);
    }
}

Data* greater_than(Data* left, Data* right) {
    if (left->type != right->type) {
        fprintf(stderr, "Cannot > two values of different types\n");
        exit(EXIT_FAILURE);
    }
    switch (left->type) {
        case T_NUMBER:
            return make_data_number(left->number.value > right->number.value);
        default:
            fprintf(stderr, "> only defined for numbers\n");
            exit(EXIT_FAILURE);
    }
}

Data* less_than(Data* left, Data* right) {
    if (left->type != right->type) {
        fprintf(stderr, "Cannot < two values of different types\n");
        exit(EXIT_FAILURE);
    }
    switch (left->type) {
        case T_NUMBER:
            return make_data_number(left->number.value < right->number.value);
        default:
            fprintf(stderr, "< only defined for numbers\n");
            exit(EXIT_FAILURE);
    }
}

Data* not(Data* value) {
    return make_data_number(!is_true(value));
}

Data* equal(Data* left, Data* right) {
    if (left->type != right->type) {
        return make_data_number(0);
    }
    switch (left->type) {
        case T_NUMBER:
            return make_data_number(left->number.value == right->number.value);
        case T_TEXT:
            return make_data_number(!strcmp(left->text.value, right->text.value));
        case T_DEVICE:
            return make_data_number(!strcmp(left->device.name, right->device.name));
        default:
            return make_data_number(0);
    }
}

Data* greater_than_or_equal(Data* left, Data* right) {
    if (left->type != right->type) {
        fprintf(stderr, "Cannot >= two values of different types\n");
        exit(EXIT_FAILURE);
    }
    switch (left->type) {
        case T_NUMBER:
            return make_data_number(left->number.value >= right->number.value);
        default:
            fprintf(stderr, ">= only defined for numbers\n");
            exit(EXIT_FAILURE);
    }
}

Data* less_than_or_equal(Data* left, Data* right) {
    if (left->type != right->type) {
        fprintf(stderr, "Cannot <= two values of different types\n");
        exit(EXIT_FAILURE);
    }
    switch (left->type) {
        case T_NUMBER:
            return make_data_number(left->number.value <= right->number.value);
        default:
            fprintf(stderr, "<= only defined for numbers\n");
            exit(EXIT_FAILURE);
    }
}

Data* not_equal(Data* left, Data* right) {
    Data* result = equal(left, right);
    result->number.value = !result->number.value;
    return result;
}

Data* multiply(Data* left, Data* right) {
    if (left->type != right->type) {
        fprintf(stderr, "Cannot multiply two values of different types\n");
        exit(EXIT_FAILURE);
    }
    switch (left->type) {
        case T_NUMBER:
            return make_data_number(left->number.value * right->number.value);
        default:
            fprintf(stderr, "Multiply only defined for numbers\n");
            exit(EXIT_FAILURE);
    }
}

Data* divide(Data* left, Data* right) {
    if (left->type != right->type) {
        fprintf(stderr, "Cannot divide two values of different types\n");
        exit(EXIT_FAILURE);
    }
    switch (left->type) {
        case T_NUMBER:
            return make_data_number(left->number.value / right->number.value);
        default:
            fprintf(stderr, "Divide only defined for numbers\n");
            exit(EXIT_FAILURE);
    }
}

Data* add_c_definition(Data* function, void* pointer, unsigned int argument_count) {
    function->function.definition_count++;
    function->function.definitions = realloc(function->function.definitions, function->function.definition_count * sizeof(T_Function));
    int index = function->function.definition_count - 1;
    function->function.definitions[index].is_c = true;
    function->function.definitions[index].c_function = malloc(sizeof(CFunction));
    function->function.definitions[index].c_function->argument_count = argument_count;
    function->function.definitions[index].c_function->function = pointer;
    return function;
}

Data* c_function(void* pointer, unsigned int argument_count) {
    Data* function = make_data_function(NULL);
    function->function.definitions[0].is_c = true;
    function->function.definitions[0].c_function = malloc(sizeof(CFunction));
    function->function.definitions[0].c_function->argument_count = argument_count;
    function->function.definitions[0].c_function->function = pointer;
    return function;
}

void import_prelude(Stack* stack) {
    add(stack, "say", c_function(say, 1));
    add(stack, "ask", c_function(ask, 1));
    add(stack, "trim", c_function(text_trim, 2));
    add(stack, "error", c_function(throw, 1));
    add(stack, "and", c_function(and, 2));
    add(stack, "or", c_function(or, 2));
    add(stack, "+", c_function(addition, 2));
    add(stack, "-", add_c_definition(c_function(subtract, 2), minus, 1));
    add(stack, "*", c_function(multiply, 2));
    add(stack, "/", c_function(divide, 2));
    add(stack, ">", c_function(greater_than, 2));
    add(stack, "<", c_function(less_than, 2));
    add(stack, ">=", c_function(greater_than_or_equal, 2));
    add(stack, "<=", c_function(less_than_or_equal, 2));
    add(stack, "==", c_function(equal, 2));
    add(stack, "/=", c_function(not_equal, 2));
    add(stack, "not", c_function(not, 1));
}
