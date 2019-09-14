#include "lexeme.h"

Lexeme* make_lexeme(LexemeType type, LexemeContent content) {
    Lexeme* lexeme = (Lexeme*) malloc(sizeof(Lexeme));
    lexeme->type = type;
    switch (type) {
        case TEXT:
        case ID:
            lexeme->string = content.string;
            break;
        case OPERATOR:
            lexeme->operator = content.operator;
            break;
        case KEYWORD:
            lexeme->keyword = content.keyword;
            break;
        case NUMBER:
            lexeme->number = content.number;
            break;
        default:
            break; // Added to resolve warnings on flow check
    }
    return lexeme;
}

void destroy_lexeme(Lexeme* lexeme) {
    if (lexeme->type == ID || lexeme->type == TEXT)
        free((void*) lexeme->string);
    free(lexeme);
}

bool lexeme_keyword(Lexeme* lexeme, Keyword keyword) {
    return (lexeme->type == KEYWORD) && lexeme->keyword == keyword;
}

bool lexeme_operator(Lexeme* lexeme, Operator operator) {
    return (lexeme->type == OPERATOR) && lexeme->operator == operator;
}

bool lexeme_string(Lexeme* lexeme, LexemeType type, const char* string) {
    return (lexeme->type == type) && !strcmp(lexeme->string, string);
}

bool lexeme_number(Lexeme* lexeme, const char* string) {
    return (lexeme->type == NUMBER) && !strcmp(lexeme->string, string);
}

bool lexeme_type_check(Lexeme* lexeme, LexemeType type) {
    return lexeme->type == type;
}

bool lexeme_check(Lexeme* lexeme, LexemeType type, LexemeContent content) {
    if (lexeme->type != type) return false;
    switch (type) {
        case TEXT:
        case ID:
            return !strcmp(lexeme->string, content.string);
        case OPERATOR:
            return lexeme->operator == content.operator;
        case KEYWORD:
            return lexeme->keyword == content.keyword;
        case NUMBER:
            return lexeme->number == content.number;
        default:
            return true;
    }
}

bool lexeme_compare(Lexeme* first, Lexeme* second) {
    if (first->type != second->type) return false;
    switch (first->type) {
        case TEXT:
        case ID:
            return !strcmp(first->string, second->string);
        case OPERATOR:
            return first->operator == second->operator;
        case KEYWORD:
            return first->keyword == second->keyword;
        case NUMBER:
            return first->number == second->number;
        default:
            return true;
    }
}

const char* lexeme_type_name(LexemeType type) {
    const char* names[] = {
            "_NONE", "NEWLINE", "INDENT", "DEDENT",
            "ID", "TEXT", "NUMBER", "OPERATOR", "KEYWORD",
            "L_BRACKET", "R_BRACKET", "L_PARENS", "R_PARENS", "COMMA"
    };
    return names[type];
}

const char* lexeme_operator_name(Operator operator) {
    const char* names[] = {
            "OP_ADD", "OP_SUBTRACT", "OP_MULTIPLY", "OP_DIVIDE", "OP_POWER",
            "OP_AND", "OP_OR", "OP_EQUAL", "OP_NOT_EQUAL", "OP_LESS_THAN",
            "OP_GREATER_THAN", "OP_LESS_THAN_EQUAL", "OP_GREATER_THAN_EQUAL",
            "OP_ASSIGN"
    };
    return names[operator];
}

const char* lexeme_operator_symbol(Operator operator) {
    const char* names[] = {
            "+", "-", "*", "/", "**",
            "and", "or", "==", "/=", "<",
            ">", "<=", ">=",
            "="
    };
    return names[operator];
}

const char* lexeme_keyword_name(Keyword keyword) {
    const char* names[] = {
            "KEYWORD_DO", "KEYWORD_LET", "KEYWORD_WITH",
            "KEYWORD_IF", "KEYWORD_WHILE"
    };
    return names[keyword];
}

const char* lexeme_to_s(Lexeme* lexeme) {
    const char* type_name = lexeme_type_name(lexeme->type);
    char* repr;
    const char* name;
    switch (lexeme->type) {
        case TEXT:
        case ID:
            repr = malloc(strlen(lexeme->string) + strlen(type_name) + 5);
            sprintf(repr, "%s(%s)", type_name, lexeme->string);
            break;
        case OPERATOR:
            name = lexeme_operator_name(lexeme->operator);
            repr = malloc(strlen(name) + strlen(type_name) + 5);
            sprintf(repr, "%s(%s)", type_name, name);
            break;
        case KEYWORD:
            name = lexeme_keyword_name(lexeme->keyword);
            repr = malloc(strlen(name) + strlen(type_name) + 5);
            sprintf(repr, "%s(%s)", type_name, name);
            break;
        case NUMBER:
            repr = malloc(20 + strlen(type_name) + 5); // 20 enough for floats?
            sprintf(repr, "%s(%f)", type_name, lexeme->number);
            break;
        default:
            repr = malloc(strlen(type_name) + 3);
            sprintf(repr, "%s", type_name);
    }
    return repr;
}
