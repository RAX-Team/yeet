#pragma once

#include <stdio.h>

#include "lexeme.h"
#include "lexemestream.h"
#include "syntaxtree.h"

typedef struct {
    ASTNode* tree;
    unsigned int consumed;
    bool success;
} ParseResult;

#define PARSE_FAILURE ((ParseResult) { .success = false })
#define propogate_fail_do(result, code)\
    do {\
        if(!(result).success) { code return PARSE_FAILURE; }\
    } while(false)
#define propogate_fail(result)\
    do {\
        if(!(result).success) { return PARSE_FAILURE; }\
    } while(false)


ParseResult success(ASTNode*, unsigned int);
int is_magic(const char*);
int scan_line_content(LexemeStream*, LexemeType, LexemeContent);
int scan_line(LexemeStream*, LexemeType);
ParseResult atom(LexemeStream*);
ParseResult function_call(LexemeStream*);
ParseResult infix_operator(LexemeStream*);
ParseResult expression(LexemeStream*);
ParseResult statement(LexemeStream*);
ParseResult block(LexemeStream*);
ParseResult function_assignment(LexemeStream*);
ParseResult lambda_function(LexemeStream*);
Programme* parse(LexemeStream*);
ParseResult control_flow(LexemeStream*);