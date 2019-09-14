#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "../parser/syntaxtree.h"
#include "stack.h"

typedef enum {
    T_FUNCTION, T_NUMBER, T_TEXT, T_DEVICE
} DataType;

typedef struct {
    unsigned int argument_count;
    void* function;
} CFunction;

typedef struct {
    bool is_c;
    union {
        Function* function;
        CFunction* c_function;
    };
} FunctionDefinition;

typedef struct {
    FunctionDefinition* definitions;
    unsigned int definition_count;
    struct Stack** stacks;
} T_Function;

typedef struct {
    float value;
} T_Number;

typedef struct {
    const char* value;
} T_Text;

typedef struct {
    const char* name;
} T_Device;

typedef struct {
    DataType type;
    union {
        T_Function function;
        T_Number number;
        T_Text text;
        T_Device device;
    };
} Data;

Data* make_data_number(float);
Data* make_data_text(const char*);
Data* make_data_function(Function*);
Data* make_data_device(const char*);
void function_merge(T_Function*, T_Function*);

void print_data(Data*);
bool number_equals(float, float);
bool is_true(Data*);