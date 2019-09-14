#include "lexer.h"

bool strprefix(const char* prefix, char* word) {
    return !strncmp(prefix, word, strlen(prefix));
}

char* strtake(const char* string, unsigned int n) {
    char* substring = malloc(sizeof(char) * (n + 1));
    strncpy(substring, string, n);
    substring[n] = '\0';
    return substring;
}

bool not_divider(char character) {
    return isalnum(character) || character == '"';
}

bool undivided_next_lexeme(const char* string, unsigned int index) {
    return not_divider(string[index - 1]) && not_divider(string[index]);
}

unsigned int keyword(LexemeStream* stream, char* text) {
    LexemeContent content;
    unsigned int length = 0;
    if (strprefix("do", text)) {
        content.keyword = KEYWORD_DO;
        length = 2;
    } else if (strprefix("with", text)) {
        content.keyword = KEYWORD_WITH;
        length = 4;
    } else if (strprefix("let", text)) {
        content.keyword = KEYWORD_LET;
        length = 3;
    } else if (strprefix("if", text)) {
        content.keyword = KEYWORD_IF;
        length = 2;
    } else if (strprefix("while", text) ) {
        content.keyword = KEYWORD_WHILE;
        length = 5;
    } else if (strprefix("whilst", text)) {
        content.keyword = KEYWORD_WHILE;
        length = 6;
    } else {
        return NO_MATCH;
    }
    if (undivided_next_lexeme(text, length)) {
        return NO_MATCH;
    }
    ls_append(stream, make_lexeme(KEYWORD, content));
    return length;
}

unsigned int operator_identifier(LexemeStream* stream, char* text) {
    int length;
    if (strprefix("(and)", text)) {
        length = 3;
    } else if (strprefix("(or)", text)
            || strprefix("(==)", text)
            || strprefix("(/=)", text)
            || strprefix("(<=)", text)
            || strprefix("(>=)", text)
            || strprefix("(**)", text)) {
        length = 2;
    } else if (strprefix("(+)", text)
            || strprefix("(*)", text)
            || strprefix("(-)", text)
            || strprefix("(/)", text)
            || strprefix("(>)", text)
            || strprefix("(<)", text)) {
        length = 1;
    } else {
        return 0;
    }
    ls_append(stream, make_lexeme(
            ID, (LexemeContent) { .string = strtake(text + 1, length) }
            ));
    return length + 2;
}

unsigned int operator(LexemeStream* stream, char* text) {
    LexemeContent content;
    unsigned int length = 0;
    if (strprefix("**", text)) {
        content.operator = OP_POWER;
        length = 2;
    } else if (strprefix("and", text)) {
        content.operator = OP_AND;
        length = 3;
    } else if (strprefix("or", text)) {
        content.operator = OP_OR;
        length = 2;
    } else if (strprefix("==", text)) {
        content.operator = OP_EQUAL;
        length = 2;
    } else if (strprefix("/=", text)) {
        content.operator = OP_NOT_EQUAL;
        length = 2;
    } else if (strprefix("<=", text)) {
        content.operator = OP_LESS_THAN_EQUAL;
        length = 2;
    } else if (strprefix(">=", text)) {
        content.operator = OP_GREATER_THAN_EQUAL;
        length = 2;
    } else if (*text == '+') {
        content.operator = OP_ADD;
        length = 1;
    } else if (*text == '*') {
        content.operator = OP_MULTIPLY;
        length = 1;
    } else if (*text == '-') {
        content.operator = OP_SUBTRACT;
        length = 1;
    } else if (*text == '/') {
        content.operator = OP_DIVIDE;
        length = 1;
    } else if (*text == '>') {
        content.operator = OP_GREATER_THAN;
        length = 1;
    } else if (*text == '<') {
        content.operator = OP_LESS_THAN;
        length = 1;
    } else if (*text == '=') {
        content.operator = OP_ASSIGN;
        length = 1;
    } else {
        return NO_MATCH;
    }
    if (undivided_next_lexeme(text, length)) {
        return NO_MATCH;
    }
    ls_append(stream, make_lexeme(OPERATOR, content));
    return length;
}

unsigned int bracket(LexemeStream* stream, const char* text) {
    LexemeType type;
    switch (*text) {
        case '(':
            type = L_PARENS;
            break;
        case ')':
            type = R_PARENS;
            break;
        case '[':
            type = L_BRACKET;
            break;
        case ']':
            type = R_BRACKET;
            break;
        default:
            return NO_MATCH;
    }
    ls_append(stream, make_lexeme(type, LC_EMPTY));
    return 1;
}

unsigned int separator(LexemeStream* stream, const char* text) {
    LexemeType type;
    switch (*text) {
        case ',':
            type = COMMA;
            break;
        default:
            return NO_MATCH;
    }
    ls_append(stream, make_lexeme(type, LC_EMPTY));
    return 1;
}

unsigned int text(LexemeStream* stream, char* text) {
    int length = 0;
    if (*text != '"') {
        return NO_MATCH;
    } else
        for (char* cursor = text + 1; *cursor != '"'; cursor++) {
            if (*cursor == '\\') {
                cursor++;
                length++;
            }
            length++;
        }
    if (undivided_next_lexeme(text, length + 2)) {
        return NO_MATCH;
    }
    ls_append(
            stream,
            make_lexeme(
                    TEXT,
                    (LexemeContent) {.string = strtake(text + 1, length)}
            )
    );
    return length + 2;
}

unsigned int number(LexemeStream* stream, char* text) {
    int length = 1;
    bool had_point = false;
    if (!isdigit(*text) && *text != '-') {
        return NO_MATCH;
    } else for (char* cursor = text + 1;
                isdigit(*cursor) || (*cursor == '.' && !had_point);
                cursor++) {
        if (*cursor == '.') had_point = true;
        length++;
    }
    if (undivided_next_lexeme(text, length)) {
        return NO_MATCH;
    }
    char* ascii = strtake(text, length);
    LexemeContent content = {.number = atof(ascii)};
    free(ascii);
    ls_append(stream, make_lexeme(NUMBER, content));
    return length;
}

bool identifier_head_character(char character) {
    return isalpha(character) || character == '\'' || character == '_';
}

bool identifier_tail_character(char character) {
    return isalnum(character) || character == '\'' || character == '_';
}

unsigned int identifier(LexemeStream* stream, char* text) {
    int length = 1;
    if (!identifier_head_character(*text)) {
        return NO_MATCH;
    } else for (
            char* cursor = text + 1;
            identifier_tail_character(*cursor);
            cursor++) {
        length++;
    }
    if (undivided_next_lexeme(text, length)) {
        return NO_MATCH;
    }
    ls_append(
            stream,
            make_lexeme(
                    ID,
                    (LexemeContent) {.string = strtake(text, length)}
            )
    );
    return length;
}

bool is_comment(char* string) {
    return strprefix("--", string) || strprefix("{-", string);
}

int skip_comment(char* string) {
    int length = 0;
    if (strprefix("--", string)) {
        for (;*string && *string != '\n'; length++, string++);
    } else if (strprefix("{-", string)) {
        string += 2;
        length = 2;
        for (int depth = 1; depth > 0; string++) {
            if (!string[1]) {
                fprintf(stderr, "Warning: Unclosed comment %s\n", string + 1);
                return length + 1;
            } else if (strprefix("-}", string)) {
                depth--;
                string++;
                length += 2;
            } else if (strprefix("{-", string)) {
                depth++;
                string++;
                length += 2;
            } else {
                length += 1;
            }
        }
    }
    return length;
}

int indent(LexemeStream* stream, const char* text,
           unsigned int* base, unsigned int* depth) {
    unsigned int size = 0, count = 0;
    for(const char* cursor = text;
            *cursor == '\n' || *cursor == ' ' || is_comment((char*) cursor);
            cursor++) {
        if (is_comment((char*) cursor)) {
            int comment_size = skip_comment((char*) cursor);
            cursor += comment_size;
            count += comment_size;
        }
        if (*cursor == '\n') size = 0;
        else size++;
        count++;
    }

    if (!*base) {
        if (size) {
            *base = size;
            *depth = 1;
            ls_append(stream, make_lexeme(INDENT, LC_EMPTY));
        }
        return count;
    }

    if (size % *base) {
        char area[11];
        strncpy(area, text, 10);
        fflush(stdin);
        fprintf(stderr, "Uneven Indentation around `%s`\n"
                        "Expected %u based spacing but found indent of size %u "
                        "where indentation was previously %u.",
                area, *base, size, *depth);
        exit(-1);
    }

    unsigned int new_depth = size / *base;
    if (new_depth > *depth) {
        if (new_depth != *depth + 1) {
            char area[11];
            strncpy(area, text, 10);
            fflush(stdin);
            fprintf(stderr, "Multiple Indents at Once around `%s`\n"
                            "A jump to indentation depth %u from %u "
                            "is not possible, as only one indent can be made "
                            "at one point.",
                    area, new_depth, *depth);
            exit(-1);
        }
        ls_append(stream, make_lexeme(INDENT, LC_EMPTY));
    } else if (new_depth < *depth) {
        for (int i = *depth; i > new_depth; i--) {
            ls_append(stream, make_lexeme(DEDENT, LC_EMPTY));
        }
    }
    *depth = new_depth;
    return count;
}

LexemeStream* lex(char* code) {
    unsigned int depth = 0;
    unsigned int indent_base = 0;
    unsigned int bracket_depth = 0;
    LexemeStream* stream = make_lexemestream();

    for (char* cursor = code; *cursor; cursor++) {
        unsigned int result;
        if (*cursor == ' ' || (bracket_depth && *cursor == '\n')) {
            continue;
        } else if (is_comment(cursor)) {
            cursor += skip_comment(cursor) - 1;
        } else if (*cursor == '\n') {
            if (!ls_empty(stream)) ls_append(stream, make_lexeme(NEWLINE, LC_EMPTY));
            cursor += indent(stream, cursor, &indent_base, &depth) - 1;
        } else if ( (result = operator_identifier(stream, cursor)) ) {
            // Due to their special syntax, these need special lexing before brackets
            cursor += result - 1;
        } else if (bracket(stream, cursor)) {
            switch (stream->end->lexeme->type) {
                case L_BRACKET:
                case L_PARENS:
                    bracket_depth++;
                    break;
                case R_BRACKET:
                case R_PARENS:
                    if (!bracket_depth) {
                        fflush(stdin);
                        fprintf(stderr,
                                "Unopened Bracket from `%s`\n"
                                "A right %c was closed without being opened, "
                                "noting there is no check to ensure brackets "
                                "are of the same type.",
                                cursor, *cursor);
                        exit(-1);
                    }
                    bracket_depth--;
                    break;
                default:
                    fflush(stdin);
                    fprintf(stderr,
                            "Unexpected bracket type from `%s`\n"
                            "The lexer found a bracket %c, but it turned out "
                            "not to be a bracket.",
                            cursor, *cursor);
                    exit(-1);
            }
        } else if ((result = keyword(stream, cursor))
                   || (result = separator(stream, cursor))
                   || (result = operator(stream, cursor))
                   || (result = text(stream, cursor))
                   || (result = number(stream, cursor))
                   || (result = identifier(stream, cursor))) {
            cursor += result - 1;
        } else {
            char area[11];
            strncpy(area, cursor, 10);
            fflush(stdin);
            fprintf(stderr, "Unidentified Lexeme `%s` %d %c\n", area, *cursor, *cursor);
            exit(-1);
        }
    }
    return stream;
}
