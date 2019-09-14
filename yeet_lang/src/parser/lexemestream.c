#include "lexemestream.h"

LexemeStream* make_lexemestream(void) {
    LexemeStream* stream = malloc(sizeof(LexemeStream));
    stream->head = NULL;
    stream->end = NULL;
    return stream;
}

void destroy_lexemestream(LexemeStream* stream) {
    for (LexemeStreamNode* node = stream->head; node; node = node->next) {
        //free(node);
    }
    free(stream);
}

LexemeStreamNode* make_ls_node(Lexeme* lexeme) {
    LexemeStreamNode* node = malloc(sizeof(LexemeStreamNode));
    node->lexeme = lexeme;
    node->next = NULL;
    return node;
}

unsigned int ls_len(LexemeStream* stream) {
    int count = 0;
    ls_node_for(stream, node) count++;
    return count;
}

void ls_append(LexemeStream* stream, Lexeme* lexeme) {
    if (!stream->head) {
        stream->head = stream->end = make_ls_node(lexeme);
    } else {
        LexemeStreamNode* node = make_ls_node(lexeme);
        stream->end->next = node;
        stream->end = node;
    }
}

LexemeStreamNode* ls_get_node(LexemeStream* stream, unsigned int index) {
    LexemeStreamNode* node = stream->head;
    for (; node && index; index--) {
        node = node->next;
    }
    return node;
}

Lexeme* ls_get(LexemeStream* stream, unsigned int index) {
    LexemeStreamNode* node = ls_get_node(stream, index);
    return node ? node->lexeme : NULL;
}

inline bool ls_empty(LexemeStream* stream) {
    return stream->head == NULL;
}

LexemeStream* ls_new_section(LexemeStream* stream,
                             unsigned int start, unsigned int end) {
    LexemeStream* substream = make_lexemestream();
    if (start >= end) return substream;
    LexemeStreamNode* node = ls_get_node(stream, start);
    if (!node) return substream;
    substream->head = make_ls_node(node->lexeme);
    node = substream->head;
    for (int i = start + 1; i < end && ls_get(stream, i); i++, node = node->next) {
        node->next = make_ls_node(ls_get(stream, i));
    }
    substream->end = node;
    return substream;
}

LexemeStream* ls_tail(LexemeStream* stream, unsigned int start) {
    LexemeStream* substream = make_lexemestream();
    substream->head = ls_get_node(stream, start);
    substream->end = (substream->head) ? stream->end : NULL;
    return substream;
}

/* Note this creates a very shallow stack_copy, like another reference */
LexemeStream* ls_copy(LexemeStream* stream) {
    LexemeStream* copy = make_lexemestream();
    copy->head = stream->head;
    copy->end = stream->end;
    return copy;
}

inline void ls_drop(LexemeStream* stream, unsigned int count) {
    stream->head = ls_get_node(stream, count);
    if (!stream->head) stream->end = NULL;
}

inline Lexeme* ls_head(LexemeStream* stream) {
    if (!stream->head) {
        return NULL;
    }
    return stream->head->lexeme;
}

inline bool ls_expr_end(LexemeStream* stream) {
    return ls_empty(stream)
            || lexeme_type_check(ls_head(stream), NEWLINE)
            || lexeme_type_check(ls_head(stream), R_PARENS)
            || lexeme_type_check(ls_head(stream), COMMA);
}

inline bool ls_block_end(LexemeStream* stream) {
    // FIXME change the lexer to emit DEDENTs at the end of a programme and remove ls_empty check
    return ls_empty(stream) || lexeme_type_check(ls_head(stream), DEDENT);
}

void ls_foreach(LexemeStream* stream, void (* map)(Lexeme*)) {
    for (LexemeStreamNode* node = stream->head; node; node = node->next) {
        map(node->lexeme);
    }
}

void ls_cleanup(LexemeStream* stream) {
    ls_foreach(stream, destroy_lexeme);
    destroy_lexemestream(stream);
}

void ls_print(LexemeStream* stream) {
    for (LexemeStreamNode* node = stream->head; node; node = node->next) {
        const char* repr = lexeme_to_s(node->lexeme);
        printf("%s ", repr);
        free((void*) repr);
        if (node->lexeme->type == NEWLINE) puts("");
    }
}