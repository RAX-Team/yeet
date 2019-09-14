#pragma once

#include "lexeme.h"

typedef struct ASTNode ASTNode;

typedef enum {
    FUNCTION, FUNCTION_CALL, DECLARATION, ASSIGNMENT, IDENTIFIER,
    LITERAL, BLOCK, CONTROL_FLOW
} ASTNodeType;

typedef enum {
    L_NUMBER, L_TEXT, L_MAGIC
} LiteralType;

typedef struct {
    LiteralType type;
    union {
        float number;
        char* string;
    };
} Literal;

typedef struct {
    ASTNode** statements;
    unsigned int statement_count;
} Block;

typedef struct {
    ASTNode* target;
    ASTNode* expression;
} Assignment;

typedef struct {
    ASTNode* name;
} Declaration;

typedef struct {
    ASTNode* function;
    unsigned int argument_count;
    ASTNode** arguments;
} FunctionCall;

typedef struct {
    unsigned int argument_count;
    ASTNode** arguments;
    Block* block;
} Function;

typedef struct {
    char* name;
} Identifier;

typedef enum {
    CONTROL_IF, CONTROL_WHILE
} ControlFlowType;

typedef struct {
    ControlFlowType type;
    ASTNode* expression;
    Block* block;
} ControlFlow;

typedef union {
    Assignment assignment;
    Declaration declaration;
    Function function;
    FunctionCall call;
    Identifier identifier;
    Literal literal;
    Block block;
    ControlFlow control;
} ASTNodeValue;

struct ASTNode {
    ASTNodeType type;
    union {
        ASTNodeValue value;
        union {
            Assignment assignment;
            Declaration declaration;
            Function function;
            FunctionCall call;
            Identifier identifier;
            Literal literal;
            Block block;
            ControlFlow control;
        };
    };
};

typedef struct {
    ASTNode** statements;
    unsigned int statement_count;
} Programme;

#define EMPTY_AST_VALUE ((ASTNodeValue){})

ASTNode* make_node(ASTNodeType, ASTNodeValue);
void destroy_node(ASTNode*);
ASTNode* make_text_node(char*);
ASTNode* make_magic_literal_node(char*);
ASTNode* make_number_node(float);
ASTNode* make_identifier_node(char*);
ASTNode* make_assignment_node(ASTNode*, ASTNode*);
ASTNode* make_function_node(unsigned int, ASTNode**, Block*);
ASTNode* make_declaration_node(ASTNode*);
ASTNode* make_call_node(ASTNode*, unsigned int, ASTNode**);
ASTNode* make_block_node(unsigned int, ASTNode**);
ASTNode* make_control_flow_node(ControlFlowType, ASTNode*, Block*);
void literal_print(Literal*);
void block_print(Block*);
void programme_print(Programme*);
void st_print(ASTNode*);
