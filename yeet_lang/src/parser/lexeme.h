#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

typedef enum {
    _ERROR = -1, _NONE = 0,
    NEWLINE, INDENT, DEDENT,
    ID,
    TEXT, NUMBER,
    OPERATOR, KEYWORD,
    L_BRACKET, R_BRACKET, L_PARENS, R_PARENS, COMMA
} LexemeType;

typedef enum {
    OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE, OP_POWER, OP_AND, OP_OR,
    OP_EQUAL, OP_NOT_EQUAL, OP_LESS_THAN, OP_GREATER_THAN, OP_LESS_THAN_EQUAL,
    OP_GREATER_THAN_EQUAL, OP_ASSIGN
} Operator;

typedef enum {
    KEYWORD_DO, KEYWORD_LET, KEYWORD_WITH, KEYWORD_IF, KEYWORD_WHILE
} Keyword;

typedef union {
    Operator operator;
    Keyword keyword;
    float number;
    char* string;
} LexemeContent;

#define LC_EMPTY ((LexemeContent) {})

typedef struct {
    LexemeType type;
    union {
        Operator operator;
        Keyword keyword;
        float number;
        char* string;
    };
} Lexeme;

Lexeme* make_lexeme(LexemeType, LexemeContent);
void destroy_lexeme(Lexeme*);
bool lexeme_check(Lexeme*, LexemeType, LexemeContent);
bool lexeme_keyword(Lexeme*, Keyword);
bool lexeme_operator(Lexeme*, Operator);
const char* lexeme_operator_symbol(Operator);
bool lexeme_string(Lexeme*, LexemeType, const char*);
bool lexeme_number(Lexeme*, const char*);
bool lexeme_type_check(Lexeme*, LexemeType);
bool lexeme_compare(Lexeme*, Lexeme*);
const char* lexeme_to_s(Lexeme*);