#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    // Program structure
    PROGRAM_NODE,
    VAR_DECL_NODE,
    VAR_DECL_LIST_NODE,
    METHOD_DECL_LIST_NODE,
    METHOD_DECL_NODE,
    EXTERN_METHOD_DECL_NODE,
    PARAM_LIST_NODE,
    PARAM_NODE,
    BLOCK_NODE,
    STATEMENT_LIST_NODE,
    
    // Statements
    ASSIGN_STMT_NODE,
    METHOD_CALL_STMT_NODE,
    IF_STMT_NODE,
    WHILE_STMT_NODE,
    RETURN_STMT_NODE,
    EMPTY_STMT_NODE,
    
    // Expressions
    IDENTIFIER_NODE,
    METHOD_CALL_NODE,
    BINARY_OP_NODE,
    UNARY_OP_NODE,
    ARITH_OP_NODE,
    REL_OP_NODE,
    COND_OP_NODE,
    
    // Literals
    INTEGER_LITERAL_NODE,
    BOOL_LITERAL_NODE,
    
    // Types
    INTEGER_TYPE_NODE,
    BOOL_TYPE_NODE,
    VOID_TYPE_NODE,
    
    // Argument lists
    ARG_LIST_NODE,
    
    // Empty node
    EMPTY_NODE
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    int int_value;
    char* string_value;
    struct ASTNode** children;
    int child_count;
} ASTNode;

// Function declarations
ASTNode* create_ast_node(ASTNodeType type, int int_value, char* string_value);
void free_ast_node(ASTNode* node);
void print_ast(ASTNode* node, int depth);
const char* get_node_type_name(ASTNodeType type);

#endif
