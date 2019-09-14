#pragma once

#include "lexeme.h"

#define ls_node_for(lexeme_stream, node_name) for(LexemeStreamNode* node_name = (lexeme_stream)->head; (node_name); (node_name) = (node_name)->next)

typedef struct LexemeStreamNode {
    Lexeme* lexeme;
    struct LexemeStreamNode* next;
} LexemeStreamNode;

typedef struct {
    LexemeStreamNode* head;
    LexemeStreamNode* end;
} LexemeStream;

LexemeStream* make_lexemestream(void);
void destroy_lexemestream(LexemeStream*);
void ls_append(LexemeStream*, Lexeme*);
Lexeme* ls_get(LexemeStream*, unsigned int);
bool ls_empty(LexemeStream*);
bool ls_expr_end(LexemeStream*);
bool ls_block_end(LexemeStream*);
unsigned int ls_len(LexemeStream*);
LexemeStreamNode* ls_node_get(LexemeStream*, unsigned int);
LexemeStream* ls_new_section(LexemeStream*, unsigned int, unsigned int);
LexemeStream* ls_tail(LexemeStream*, unsigned int);
LexemeStream* ls_copy(LexemeStream*);
Lexeme* ls_head(LexemeStream*);
void ls_drop(LexemeStream*, unsigned int);
void ls_foreach(LexemeStream*, void (* map)(Lexeme*));
void ls_cleanup(LexemeStream*);
void ls_print(LexemeStream*);