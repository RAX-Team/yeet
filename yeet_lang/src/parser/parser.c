#include "parser.h"

int is_magic(const char* name) {
    return isupper(*name);
}

int scan_line_content(LexemeStream* stream, LexemeType type, LexemeContent content) {
    unsigned int i = 0;
    for (LexemeStreamNode* cursor = stream->head;
         cursor && !lexeme_type_check(cursor->lexeme, NEWLINE);
         cursor = cursor->next, i++) {
        if (lexeme_check(cursor->lexeme, type, content)) {
            return i;
        }
    }
    return -1;
}

int scan_line(LexemeStream* stream, LexemeType type) {
    unsigned int i = 0;
    for (LexemeStreamNode* cursor = stream->head;
         cursor && !lexeme_type_check(cursor->lexeme, NEWLINE);
         cursor = cursor->next, i++) {
        if (cursor->lexeme->type == type) {
            return i;
        }
    }
    return -1;
}

inline ParseResult success(ASTNode* node, unsigned int consumed) {
    return (ParseResult) {
            .tree = node,
            .consumed = consumed,
            .success = true
    };
}

ParseResult atom_identifier(LexemeStream* stream) {
    Lexeme* head = ls_head(stream);
    if (lexeme_type_check(head, ID) && !is_magic(head->string)) {
        return success(make_identifier_node(strdup(head->string)), 1);
    }
    return PARSE_FAILURE;
}

ParseResult atom(LexemeStream* stream) {
    if (ls_empty(stream)) return PARSE_FAILURE;
    Lexeme* head = ls_head(stream);
    switch (head->type) {
        case NUMBER:
            return success(make_number_node(head->number), 1);
        case TEXT:
            return success(make_text_node(strdup(head->string)), 1);
        case ID:
            if (is_magic(head->string)) {
                return success(make_magic_literal_node(strdup(head->string)), 1);
            }
            return atom_identifier(stream);
        case L_PARENS:
            if (ls_get(stream, 1) && ls_get(stream, 1)-> type == OPERATOR) {
                if (ls_get(stream, 1)->operator == OP_SUBTRACT
                    && ls_get(stream, 2) && ls_get(stream, 2)->type == NUMBER
                    && ls_get(stream, 3) && ls_get(stream, 3)->type == R_PARENS) {
                    return success(make_number_node(-ls_get(stream, 2)->number), 4u);
                }
            }
        default:
            return PARSE_FAILURE;
    }
}

ParseResult term(LexemeStream* stream) {
    if (ls_empty(stream)) return PARSE_FAILURE;
    if (ls_head(stream)->type != L_PARENS) {
        return atom(stream);
    } else {
        if (ls_get(stream, 1) && ls_get(stream, 1)-> type == OPERATOR) {
            if (ls_get(stream, 1)->operator == OP_SUBTRACT
                    && ls_get(stream, 2) && ls_get(stream, 2)->type == NUMBER
                    && ls_get(stream, 3) && ls_get(stream, 3)->type == R_PARENS) {
                return success(make_number_node(-ls_get(stream, 2)->number), 4u);
            }
        }
        ParseResult sub_expression = expression(ls_tail(stream, 1));
        propogate_fail(sub_expression);
        ls_drop(stream, 1 + sub_expression.consumed);
        if (ls_empty(stream) || ls_head(stream)->type != R_PARENS) {
            free(sub_expression.tree);
            return PARSE_FAILURE;
        }
        sub_expression.consumed += 2;
        return sub_expression;
    }
}

ParseResult function_call(LexemeStream* stream) {
    ParseResult function = term(ls_copy(stream));
    propogate_fail(function);
    ls_drop(stream, function.consumed);
    ASTNodeValue value;
    value.call.function = function.tree;
    value.call.arguments = NULL;
    value.call.argument_count = 0;
    // TODO forced consuption of new line could be problematic. Considering a fix at lexing stage.
    int allocated_argument_space = 0;
    int consumed = function.consumed;
    for (int i = 0; !ls_expr_end(stream); i++) {
        if (value.call.argument_count == allocated_argument_space) {
            allocated_argument_space = allocated_argument_space * 2 + 1;
            value.call.arguments =
                    realloc(value.call.arguments,
                            sizeof(ASTNode*) * allocated_argument_space);
        }
        ParseResult argument = term(ls_copy(stream));
        if (!argument.success) {
            argument = lambda_function(ls_copy(stream));
            propogate_fail_do(argument, {
                free(value.call.arguments);
                free(function.tree); // TODO implement freeing previous arguments
            });
            // Handle case where lambda function ends on a newline
            if (lexeme_type_check(ls_get(stream, argument.consumed - 1), DEDENT)) {
                argument.consumed--;
                ls_get(stream, argument.consumed)->type = NEWLINE;
            }
        }
        value.call.arguments[i] = argument.tree;
        value.call.argument_count++;
        ls_drop(stream, argument.consumed);
        consumed += argument.consumed;
    }
    return success(make_node(FUNCTION_CALL, value), consumed);
}

ParseResult variable_assignment(LexemeStream* stream) {
    int position = scan_line_content(stream, OPERATOR, (LexemeContent) {.operator = OP_ASSIGN});
    // 1 for the consumed OP_ASSIGN
    int consumed = 1;
    if (position < 0) {
        return PARSE_FAILURE;
    }
    LexemeStream* left = ls_new_section(stream, 0, position);
    ls_drop(stream, position + 1);
    // FIXME Consider a generalized system for declarations
    bool declaration = lexeme_keyword(ls_head(left), KEYWORD_LET);
    if (declaration) {
        ls_drop(left, 1);
        consumed++;
    }
    ParseResult leftResult = atom_identifier(left);
    propogate_fail_do(leftResult, {
        destroy_lexemestream(left);
    });
    consumed += leftResult.consumed;
    if (ls_len(left) != leftResult.consumed) {
        destroy_lexemestream(left);
        free(leftResult.tree);
        return PARSE_FAILURE;
    }
    ASTNode* name;
    if (declaration)
        name = make_declaration_node(leftResult.tree);
    else
        name = leftResult.tree;
    destroy_lexemestream(left);
    ParseResult rightResult = expression(ls_copy(stream));
    if (!rightResult.success) {
        return PARSE_FAILURE;
    }
    propogate_fail_do(rightResult, {
        free(leftResult.tree);
    });
    consumed += rightResult.consumed;
    return success(make_assignment_node(name, rightResult.tree), consumed);
}

ParseResult infix_operator(LexemeStream* stream) {
    const Operator operator_precedence[] = {
            OP_OR, OP_AND, OP_EQUAL, OP_NOT_EQUAL,
            OP_GREATER_THAN_EQUAL, OP_LESS_THAN_EQUAL, OP_GREATER_THAN,
            OP_LESS_THAN, OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE,
            OP_POWER
    };
    const int INFIX_OPERATORS = 13;

    for (int i = 0; i < INFIX_OPERATORS; i++) {
        int position = scan_line_content(
                stream,
                OPERATOR,
                (LexemeContent) { .operator = operator_precedence[i] }
        );
        if (position < 0) {
            continue;
        }
        int consumed = 1;
        LexemeStream* left = ls_new_section(stream, 0, position);
        LexemeStream* right = ls_tail(stream, position + 1);
        ParseResult leftResult = expression(left);
        if (!leftResult.success) {
            destroy_lexemestream(left);
            continue;
        }
        consumed += leftResult.consumed;
        if (ls_len(left) != leftResult.consumed) {
            free(leftResult.tree);
            destroy_lexemestream(left); // FIXME not on Cygwin
            continue;
        }
        destroy_lexemestream(left); // FIXME not on Cygwin

        ParseResult rightResult = expression(right);
        propogate_fail_do(rightResult, {
            free(right);
        });
        consumed += rightResult.consumed;
        ASTNode** arguments = malloc(sizeof(ASTNode*) * 2);
        arguments[0] = leftResult.tree;
        arguments[1] = rightResult.tree;
        free(right);
        return success(make_call_node(
                make_identifier_node(
                        strdup(lexeme_operator_symbol(operator_precedence[i]))
                        ),
                2,
                arguments),
            consumed);
    }
}

ParseResult expression(LexemeStream* stream) {
    if (ls_empty(stream)) return PARSE_FAILURE;
    ParseResult result = variable_assignment(ls_copy(stream));
    if (result.success) {
        return result;
    }
    result = lambda_function(ls_copy(stream));
    if (result.success) {
        // Handle case where lambda function ends on a newline so DEDENT the last token
        if (lexeme_type_check(ls_get(stream, result.consumed - 1), DEDENT)) {
            result.consumed--;
            ls_get(stream, result.consumed)->type = NEWLINE;
        }
        return result;
    }
    result = function_call(ls_copy(stream));
    if (result.success) {
        return result;
    }
    result = infix_operator(ls_copy(stream));
    if (result.success) {
        return result;
    }
    return term(stream);
}

ParseResult statement(LexemeStream* stream) {
    ParseResult result = function_assignment(ls_copy(stream));
    if (result.success) {
        return result;
    }
    result = control_flow(ls_copy(stream));
    if (result.success) {
        return result;
    }
    result = expression(ls_copy(stream));
    if (ls_expr_end(ls_tail(stream, result.consumed))) {
        result.consumed +=
                ls_get(stream, result.consumed)
                && lexeme_type_check(ls_get(stream, result.consumed), NEWLINE);
        return result;
    }
    return PARSE_FAILURE;
}

ParseResult block(LexemeStream* stream) {
    if (!lexeme_type_check(ls_head(stream), INDENT)) {
        return PARSE_FAILURE;
    }
    ls_drop(stream, 1);
    ASTNode** statements = NULL;
    int count = 0, consumed = 1;
    int allocated_statement_space = 0;
    for (int i = 0; !ls_block_end(stream); i++) {
        if (count == allocated_statement_space) {
            allocated_statement_space = (allocated_statement_space * 2) + 1;
            statements = realloc(statements, sizeof(ASTNode*) * allocated_statement_space);
        }
        ParseResult result = statement(ls_copy(stream));
        propogate_fail_do(result, {
            free(statements);
        });
        consumed += result.consumed;
        ls_drop(stream, result.consumed);
        statements[i] = result.tree;
        count++;
    }
    if (!(ls_head(stream) && lexeme_type_check(ls_head(stream), DEDENT))) {
        fprintf(stderr, "Blocks must be de-dented");
        free(statements); // TODO free all allocated statement nodes
        return PARSE_FAILURE;
    }
    consumed++;
    return success(make_block_node(count, statements), consumed);
}

ParseResult function_arguments(LexemeStream* stream) {
    ASTNodeValue value;
    value.function.arguments = NULL;
    value.function.argument_count = 0;
    int allocated_argument_space = 0;
    int consumed = 0;
    for (int i = 0; !ls_expr_end(stream); i++) {
        if (value.function.argument_count == allocated_argument_space) {
            allocated_argument_space = allocated_argument_space * 2 + 1;
            value.function.arguments =
                    realloc(value.function.arguments,
                            sizeof(ASTNode*) * allocated_argument_space);
        }
        ParseResult argument = atom(ls_copy(stream));
        propogate_fail_do(argument, {
            // if (value.call.arguments) free(value.call.arguments); // TODO implement freeing previous arguments
        });
        value.function.arguments[i] = argument.tree;
        value.function.argument_count++;
        ls_drop(stream, argument.consumed);
        consumed += argument.consumed;
    }
    return success(make_node(FUNCTION, value), consumed);
}

ParseResult function_assignment(LexemeStream* stream) {
    int position = scan_line_content(stream, KEYWORD, (LexemeContent) {.keyword = KEYWORD_DO});
    // 1 for consuming the "do" keyword
    int consumed = 1;
    if (position < 0) {
        return PARSE_FAILURE;
    }
    LexemeStream* left = ls_new_section(stream, 0, position);
    ls_drop(stream, position + 1);
    // FIXME Consider a generalized system for declarations
    bool declaration = lexeme_keyword(ls_head(left), KEYWORD_LET);
    if (declaration) {
        ls_drop(left, 1);
        consumed++;
    }
    ParseResult functionResult = atom_identifier(ls_copy(left));
    propogate_fail_do(functionResult, {
        destroy_lexemestream(left);
    });
    ls_drop(left, functionResult.consumed);
    consumed +=  functionResult.consumed;
    ParseResult argumentsResult = function_arguments(ls_copy(left));
    propogate_fail_do(argumentsResult, {
        destroy_lexemestream(left);
    });
    consumed += argumentsResult.consumed;
    ASTNode* function = argumentsResult.tree;
//    destroy_lexemestream(left); // FIXME not on Cygwin

    ASTNode* name;
    if (declaration)
        name = make_declaration_node(functionResult.tree);
    else
        name = functionResult.tree;
    if (!lexeme_type_check(ls_head(stream), NEWLINE)) {
        ParseResult expr = statement(ls_copy(stream));
        propogate_fail_do(expr, {
            free(functionResult.tree);
            free(argumentsResult.tree);
        });
        consumed += expr.consumed;
        ASTNode** statements = malloc(sizeof(ASTNode));
        statements[0] = expr.tree;
        function->function.block = &make_block_node(1, statements)->block;
        return success(
                make_assignment_node(
                        name,
                        function
                ), consumed);
    }
    ls_drop(stream, 1);
    consumed++;

    ParseResult blockResult = block(ls_copy(stream));
    consumed += blockResult.consumed;
    propogate_fail_do(blockResult, {
        free(functionResult.tree);
        free(argumentsResult.tree);
    });
    function->function.block = &blockResult.tree->block;
    return success(
        make_assignment_node(
            name,
            function
        ), consumed);
}

ParseResult lambda_function(LexemeStream* stream) {
    if (!lexeme_check(ls_head(stream), KEYWORD, (LexemeContent) { .keyword = KEYWORD_WITH })) {
        return PARSE_FAILURE;
    }
    int position = scan_line_content(
            stream, KEYWORD, (LexemeContent) { .keyword = KEYWORD_DO });
    if (position < 0) {
        return PARSE_FAILURE;
    }
    int consumed = 2; // Consume the keywords `with` and `do`
    LexemeStream* left = ls_new_section(stream, 1, position);
    ls_drop(stream, position + 1);

    ParseResult argumentsResult = function_arguments(left);
    propogate_fail_do(argumentsResult, {
        destroy_lexemestream(left);
    });
    consumed += argumentsResult.consumed;
    ASTNode* function = argumentsResult.tree;
    destroy_lexemestream(left);

    if (!lexeme_type_check(ls_head(stream), NEWLINE)) {
        ParseResult expr = expression(ls_copy(stream));
        propogate_fail_do(expr, {
            free(argumentsResult.tree);
        });
        consumed += expr.consumed;
        ASTNode** statements = malloc(sizeof(ASTNode));
        statements[0] = expr.tree;
        function->function.block = &make_block_node(1, statements)->block;
        return success(function, consumed);
    }
    ls_drop(stream, 1);
    consumed++;

    ParseResult blockResult = block(ls_copy(stream));
    consumed += blockResult.consumed;
    propogate_fail_do(blockResult, {
        free(argumentsResult.tree);
    });
    function->function.block = &blockResult.tree->block;
    return success(function, consumed);
}

ParseResult control_flow(LexemeStream* stream) {
    if (!lexeme_type_check(ls_head(stream), KEYWORD)) {
        return PARSE_FAILURE;
    }
    ControlFlowType type;
    switch (ls_head(stream)->operator) {
        case KEYWORD_WHILE:
            type = CONTROL_WHILE;
            break;
        case KEYWORD_IF:
            type = CONTROL_IF;
            break;
        default:
            return PARSE_FAILURE;
    }
    int consumed = 1; // Consume the keywords `while` or `if`
    ls_drop(stream, 1);

    ParseResult condition = expression(stream);
    propogate_fail(condition);
    consumed += condition.consumed;
    ls_drop(stream, condition.consumed);

    if (!ls_empty(stream) && lexeme_type_check(ls_head(stream), COMMA)) {
        ls_drop(stream, 1);
        ParseResult expr = statement(ls_copy(stream));
        propogate_fail_do(expr, {
            free(condition.tree);
        });
        consumed += expr.consumed
                + !(ls_get(stream, expr.consumed) && lexeme_type_check(ls_get(stream, expr.consumed), NEWLINE));
        ASTNode** statements = malloc(sizeof(ASTNode));
        statements[0] = expr.tree;
        Block* block = &make_block_node(1, statements)->block;
        return success(make_control_flow_node(type, condition.tree, block), consumed);
    }
    if (ls_empty(stream) || !lexeme_type_check(ls_head(stream), NEWLINE)) {
        free(condition.tree);
        return PARSE_FAILURE;
    }
    ls_drop(stream, 1);
    consumed++;

    ParseResult blockResult = block(ls_copy(stream));
    consumed += blockResult.consumed;
    propogate_fail_do(blockResult, {
        free(condition.tree);
    });
    return success(make_control_flow_node(type, condition.tree, &blockResult.tree->block), consumed);
}

Programme* parse(LexemeStream* stream) {
    Programme* programme = malloc(sizeof(Programme));
    programme->statement_count = 0;
    programme->statements = NULL;
    int allocated_statement_space = 0;
    for (int i = 0; !ls_empty(stream); i++) {
        if (programme->statement_count == allocated_statement_space) {
            allocated_statement_space = allocated_statement_space * 2 + 1;
            programme->statements = realloc(
                    programme->statements,
                    sizeof(ASTNode*) * allocated_statement_space
            );
        }
        ParseResult result = statement(ls_copy(stream));
        if (!result.success) {
            fprintf(stderr, "Could not parse programme statement %d\n", i);
            printf("Lexeme stream state: "), ls_print(stream), printf("\n");
            printf("Syntax tree built: "), programme_print(programme), printf("\n");
            return NULL;
        }
        ls_drop(stream, result.consumed);
        programme->statements[i] = result.tree;
        programme->statement_count++;
    }
    return programme;
}