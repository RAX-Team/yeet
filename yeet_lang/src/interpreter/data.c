#include "data.h"

bool number_equals(float left, float right) {
    return fabs((double) (left - right)) < 0.0001;
}

bool is_true(Data* data) {
    switch (data->type) {
        case T_NUMBER:
            return data->number.value != 0;
        case T_TEXT:
            return *data->text.value != '\0';
        default:
            return true;
    }
}

Data* make_data_number(float value) {
    Data* data = malloc(sizeof(Data));
    data->type = T_NUMBER;
    data->number.value = value;
    return data;
}

Data* make_data_text(const char* text) {
    Data* data = malloc(sizeof(Data));
    data->type = T_TEXT;
    data->text.value = text;
    return data;
}

Data* make_data_device(const char* name) {
    Data* data = malloc(sizeof(Data));
    data->type = T_DEVICE;
    data->device.name = name;
    return data;
}

FunctionDefinition function_definition(Function* function) {
    FunctionDefinition definition;
    definition.function = function;
    definition.is_c = false;
    return definition;
}

Data* make_data_function(Function* function) {
    Data* data = malloc(sizeof(Data));
    data->type = T_FUNCTION;
    data->function.definitions = malloc(sizeof(FunctionDefinition*) * 10); // FIXME make this not specific to max 10
    data->function.stacks = malloc(sizeof(Stack*) * 10); // FIXME make this not specific to max 10
    data->function.definitions[0] = function_definition(function);
    data->function.definition_count = 1;
    return data;
}

void function_merge(T_Function* target, T_Function* input) {
    for (int i = 0; i < input->definition_count; i++) {
        if (input->definitions[i].is_c) {
            fprintf(stderr, "Currently cannot merge C function into other function\n");
            exit(EXIT_FAILURE);
        }
        Function* merging = input->definitions[i].function;
        bool inserted = false;
        for (int j = 0; !inserted && j < target->definition_count; j++) {
            if (target->definitions->is_c) continue;
            Function* definition = target->definitions[j].function;
            if (merging->argument_count == definition->argument_count) {
                bool match = true;
                for (int k = 0; match && k < merging->argument_count; k++) {
                    ASTNode* merging_arg = merging->arguments[k],
                            * definition_arg = definition->arguments[k];
                    if (merging_arg->type != definition_arg->type) {
                        match = false;
                    } else
                        switch (merging_arg->type) {
                            case LITERAL:
                                if (merging_arg->literal.type != definition_arg->literal.type) {
                                    match = false;
                                } else
                                    switch (merging_arg->literal.type) {
                                        case L_NUMBER:
                                            if (!number_equals(merging_arg->literal.number,
                                                               definition_arg->literal.number)) {
                                                match = false;
                                            }
                                            break;
                                        case L_TEXT:
                                        case L_MAGIC:
                                            if (strcmp(merging_arg->literal.string, definition_arg->literal.string) !=
                                                0) {
                                                match = false;
                                            }
                                    }
                                break;
                            case IDENTIFIER:
                                break; // If it's an identifier then it's still a match
                            default:
                                fprintf(stderr, "Unexpected argument type on function merge");
                                exit(EXIT_FAILURE);
                        }
                }
                if (match) {
                    inserted = true;
                    target->definitions[j].function = merging;
                    target->stacks[j] = input->stacks[i];
                }
            }
        }
        if (!inserted) {
            target->definitions[target->definition_count].function = merging;
            target->stacks[target->definition_count] = input->stacks[i];
            target->definition_count++;
        }
    }
}

void print_data(Data* data) {
    switch (data->type) {
        case T_NUMBER:
            printf("%f", data->number.value);
            break;
        case T_TEXT:
            printf("%s", data->text.value);
            break;
        case T_DEVICE:
            printf("Device<%s>", data->device.name);
            break;
        case T_FUNCTION:
            printf("Function<defs:%d>", data->function.definition_count);
            break;
    }
}
