#include "syntaxtree.h"

ASTNode* make_node(ASTNodeType type, ASTNodeValue value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type;
    node->value = value;
    return node;
}

ASTNode* make_text_node(char* string) {
    ASTNodeValue value;
    value.literal.type = L_TEXT;
    value.literal.string = string;
    return make_node(LITERAL, value);
}

ASTNode* make_magic_literal_node(char* string) {
    ASTNodeValue value;
    value.literal.type = L_MAGIC;
    value.literal.string = string;
    return make_node(LITERAL, value);
}

ASTNode* make_number_node(float number) {
    ASTNodeValue value;
    value.literal.type = L_NUMBER;
    value.literal.number = number;
    return make_node(LITERAL, value);
}

ASTNode* make_identifier_node(char* name) {
    ASTNodeValue value;
    value.identifier.name = name;
    return make_node(IDENTIFIER, value);
}

ASTNode* make_declaration_node(ASTNode* name) {
    ASTNodeValue value;
    value.declaration.name = name;
    return make_node(DECLARATION, value);
}

ASTNode* make_assignment_node(ASTNode* left, ASTNode* right) {
    ASTNodeValue value;
    value.assignment.target = left;
    value.assignment.expression = right;
    return make_node(ASSIGNMENT, value);
}

ASTNode* make_function_node(unsigned int argument_count, ASTNode** arguments, Block* block) {
    ASTNodeValue value;
    value.function.argument_count = argument_count;
    value.function.arguments = arguments;
    value.function.block = block;
    return make_node(FUNCTION, value);
}

ASTNode* make_call_node(ASTNode* function, unsigned int argument_count, ASTNode** arguments) {
    ASTNodeValue value;
    value.call.function = function;
    value.call.argument_count = argument_count;
    value.call.arguments = arguments;
    return make_node(FUNCTION_CALL, value);
}

ASTNode* make_block_node(unsigned int statement_count, ASTNode** statements) {
    ASTNodeValue value;
    value.block.statement_count = statement_count;
    value.block.statements = statements;
    return make_node(BLOCK, value);
}

ASTNode* make_control_flow_node(ControlFlowType type, ASTNode* expression, Block* block) {
    ASTNodeValue value;
    value.control.type = type;
    value.control.expression = expression;
    value.control.block = block;
    return make_node(CONTROL_FLOW, value);
}

void literal_print(Literal* literal) {
    switch (literal->type) {
        case L_NUMBER:
            printf("NUMBER(%f)", literal->number);
            break;
        case L_TEXT:
            printf("TEXT(%s)", literal->string);
            break;
        case L_MAGIC:
            printf("MAGIC(%s)", literal->string);
            break;
    }
}

void block_print(Block* block) {
    for (int i = 0; i < block->statement_count; i++) {
        printf("line %u: ", i);
        st_print(block->statements[i]);
        printf("\n");
    }
}

void programme_print(Programme* programme) {
    for (int i = 0; i < programme->statement_count; i++) {
        printf("line %u: ", i);
        st_print(programme->statements[i]);
        printf("\n");
    }
}

void st_print(ASTNode* node) {
    switch (node->type) {
        case FUNCTION_CALL:
            printf("CALL(function=");
            st_print(node->call.function);
            printf(", %d argument(s)", node->call.argument_count);
            for (int i = 0; i < node->call.argument_count; i++) {
                printf(", arg%u=", i);
                st_print(node->call.arguments[i]);
            }
            printf(")");
            break;
        case FUNCTION:
            printf("FUNCTION(");
            for (int i = 0; i < node->function.argument_count; i++) {
                printf("arg%u=", i);
                st_print(node->function.arguments[i]);
                printf(", ");
            }
            puts("block=");
            block_print(node->function.block);
            printf(")");
            break;
        case IDENTIFIER:
            printf("IDENTIFIER(%s)", node->identifier.name);
            break;
        case LITERAL:
            literal_print(&node->literal);
            break;
        case DECLARATION:
            printf("DECLARATION(name=");
            st_print(node->declaration.name);
            printf(")");
            break;
        case ASSIGNMENT:
            printf("ASSIGNMENT(target=");
            st_print(node->assignment.target);
            printf(", expr=");
            st_print(node->assignment.expression);
            printf(")");
            break;
        case BLOCK:
            block_print(&node->block);
            break;
        case CONTROL_FLOW:
            printf("CONTROL_FLOW(type=%d, expr=", node->control.type);
            st_print(node->control.expression);
            printf(", block=\n");
            block_print(node->control.block);
            printf(")");
            break;
    }
}

void destroy_node(ASTNode* node) {
    switch (node->type) {
        case FUNCTION_CALL:
            for (int i = 0; i < node->call.argument_count; i++) {
                destroy_node(node->call.arguments[i]);
            }
            destroy_node(node->call.function);
        case FUNCTION:
            // Destruction of the block might be necessary here
            for (int i = 0; i < node->function.argument_count; i++) {
                destroy_node(node->function.arguments[i]);
            }
    }
    free(node);
}

