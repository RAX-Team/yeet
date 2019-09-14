#include "interpreter.h"

Data* eval_literal(Stack* stack, Literal* literal) {
    switch (literal->type) {
        case L_NUMBER:
            return make_data_number(literal->number);
        case L_TEXT:
            return make_data_text(literal->string);
        case L_MAGIC:
            return make_data_device(literal->string);
    }
}

Data* eval_identifier(Stack* stack, Identifier* identifier) {
    Data* value = find(stack, identifier->name);
    if (!value) {
        fprintf(stderr, "Attempted access of unknown (undefined) variable `%s`\n", identifier->name);
        exit(EXIT_FAILURE);
    }
    return value;
}

Data* eval_call(Stack* stack, FunctionCall* call) {
    Data* function = evaluate(stack, call->function);
    if (function->type != T_FUNCTION) {
        if (call->argument_count == 0) {
            return evaluate(stack, call->function);
        }
        fprintf(stderr, "Attempted call of non-function value with %d argument(s)\n", call->argument_count);
        exit(EXIT_FAILURE);
    }

    Data** arguments_values = malloc(sizeof(Data*) * call->argument_count);
    for (int i = 0; i < call->argument_count; i++) {
        arguments_values[i] = evaluate(stack, call->arguments[i]);
    }

    return exec_function(&function->function, call->argument_count, arguments_values);
}

Data* eval_function(Stack* stack, Function* node) {
    stack = stack_copy(stack);
    Data* function = make_data_function(node);
    function->function.stacks[0] = (struct Stack*) stack; // FIXME it seems include loops require this?
    return function;
}

Data* evaluate(Stack* stack, ASTNode* node) {
    switch (node->type) {
        case LITERAL:
            return eval_literal(stack, &node->literal);
        case FUNCTION_CALL:
            return eval_call(stack, &node->call);
        case IDENTIFIER:
            return eval_identifier(stack, &node->identifier);
        case FUNCTION:
            return eval_function(stack, &node->function);
        default:
            fprintf(stderr, "Cannot evaluate non-expression\n");
            exit(EXIT_FAILURE);
    }
}

Data* exec_block(Stack* stack, Block* block) {
    Data* result = NULL;
    for (int i = 0; i < block->statement_count; i++) {
        ASTNode* statement = block->statements[i];
        result = execute(stack, statement);
    }
    return result;
}

Data* exec_function(T_Function* function, unsigned int argument_count, Data** arguments) {
    int selected = -1;
    int selected_specialisation = -1;

    for (int i = 0; i < function->definition_count; i++) {
        int specialisation = 0;
        if (function->definitions[i].is_c) {
            if (function->definitions[i].c_function->argument_count == argument_count) {
                specialisation = 0;
            } else {
                specialisation = -2;
            }
        } else {
            Function* definition = function->definitions[i].function;
            // DONTFIXMEFORNOW ehhh pls no but okay, if reimplemented then no because types
            if (argument_count != definition->argument_count) continue;
            for (int j = 0; specialisation >= 0 && j < definition->argument_count; j++) {
                if (definition->arguments[j]->type == LITERAL) {
                    switch (definition->arguments[j]->literal.type) {
                        case L_TEXT:
                            if (arguments[j]->type == T_TEXT
                                && !strcmp(arguments[j]->text.value, definition->arguments[j]->literal.string))
                                specialisation++;
                            else specialisation = -2;
                            break;
                        case L_MAGIC:
                            if (arguments[j]->type == T_DEVICE
                                && !strcmp(arguments[j]->text.value, definition->arguments[j]->literal.string))
                                specialisation++;
                            else specialisation = -2;
                            break;
                        case L_NUMBER:
                            if (arguments[j]->type == T_NUMBER
                                && number_equals(definition->arguments[j]->literal.number, arguments[j]->number.value))
                                specialisation++;
                            else specialisation = -2;
                    }
                }
            }
        }
        if (specialisation >= selected_specialisation) {
            selected = i;
            selected_specialisation = specialisation;
        }
    }

    if (selected < 0) {
        fprintf(stderr, "No match out of %d definitions for specified call with %d arguments",
                function->definition_count, argument_count);
        exit(EXIT_FAILURE);
    }
    if (function->definitions[selected].is_c) {
        CFunction* c_function = function->definitions[selected].c_function;
        if (c_function->argument_count == 0) {
            void* (* pointer)(void) = c_function->function;
            return pointer();
        } else if (c_function->argument_count == 1) {
            void* (* pointer)(void*) = c_function->function;
            return pointer(arguments[0]);
        } else if (c_function->argument_count == 2) {
            void* (* pointer)(void*, void*) = c_function->function;
            return pointer(arguments[0], arguments[1]);
        }
    } else {
        Function* node = function->definitions[selected].function;

        Stack* stack = (Stack*) function->stacks[selected];
        push(stack);
        for (int i = 0; i < argument_count; i++) {
            if (node->arguments[i]->type == IDENTIFIER) {
                add(stack, node->arguments[i]->identifier.name, arguments[i]);
            }
        }

        Data* result = exec_block((Stack*) function->stacks[selected], node->block);
        pop(stack);
        return result;
    }
}

Data* exec_assignment(Stack* stack, Assignment* assignment) {
    Entry* target = NULL;
    if (assignment->target->type == DECLARATION) {
        if (assignment->target->declaration.name->type != IDENTIFIER) {
            fprintf(stderr, "Attempted to declare something that's not a name\n");
            exit(EXIT_FAILURE);
        }
        target = add(stack, assignment->target->declaration.name->identifier.name, NULL);
        if (!target) {
            fprintf(stderr, "Attempted to declare already declared name `%s`\n",
                    assignment->target->declaration.name->identifier.name);
            exit(EXIT_FAILURE);
        }
    } else if (assignment->target->type == IDENTIFIER) {
        target = find_entry(stack, assignment->target->identifier.name);
        if (!target) {
            fprintf(stderr, "Attempted to assign to undeclared name `%s`\n",
                    assignment->target->identifier.name);
            exit(EXIT_FAILURE);
        }
    } else {
        if (assignment->target->declaration.name->type != IDENTIFIER) {
            fprintf(stderr, "Attempted to assign to not a name\n");
            exit(EXIT_FAILURE);
        }
    }

    Data* expression = execute(stack, assignment->expression);

    if (expression->type == T_FUNCTION && target->value && ((Data*) target->value)->type == T_FUNCTION) {
        function_merge(&((Data*) target->value)->function, &expression->function);
    } else {
        target->value = expression;
    }
    return target->value;
}

Data* exec_while(Stack* stack, ControlFlow* control) {
    Data* result = NULL;
    while (is_true(evaluate(stack, control->expression))) {
        push(stack);
        result = exec_block(stack, control->block);
        pop(stack);
    }
    if (result) return result;
    else return make_data_number(0);
}

Data* exec_if(Stack* stack, ControlFlow* control) {
    if (is_true(evaluate(stack, control->expression))) {
        push(stack);
        Data* result = exec_block(stack, control->block);
        pop(stack);
        return result;
    }
    return make_data_number(0);
}

Data* exec_control(Stack* stack, ControlFlow* control) {
    switch (control->type) {
        case CONTROL_WHILE:
            return exec_while(stack, control);
        case CONTROL_IF:
            return exec_if(stack, control);
        default:
            fprintf(stderr, "Unrecognised control command 0x%08x", control->type);
            exit(EXIT_FAILURE);
    }
}

Data* execute(Stack* stack, ASTNode* statement) {
    switch (statement->type) {
        case ASSIGNMENT:
            return exec_assignment(stack, &statement->assignment);
        case CONTROL_FLOW:
            return exec_control(stack, &statement->control);
        default:
            return evaluate(stack, statement);
    }
}

Stack* run(Programme* programme) {
    Stack* stack = make_stack();
    push(stack);
    import_prelude(stack);
    import_io(stack);
    for (int i = 0; i < programme->statement_count; i++) {
        ASTNode* statement = programme->statements[i];
        Data* line_result = execute(stack, statement);
        DEBUGGING if (line_result) printf("%d: ", i), print_data(line_result), puts("");
    }
    return stack;
}