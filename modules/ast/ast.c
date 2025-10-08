#define _GNU_SOURCE
#include "ast.h"

#ifndef strdup
char* strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* dup = malloc(len);
    if (dup) {
        memcpy(dup, s, len);
    }
    return dup;
}
#endif

ASTNode* create_ast_node(ASTNodeType type, int int_value, char* string_value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el nodo AST\n");
        exit(1);
    }
    
    node->type = type;
    node->int_value = int_value;
    if (string_value) {
        node->string_value = strdup(string_value);
    } else {
        node->string_value = NULL;
    }
    node->children = NULL;
    node->child_count = 0;
    
    return node;
}

void free_ast_node(ASTNode* node) {
    if (!node) return;
    
    // Liberar string_value si existe
    if (node->string_value) {
        free(node->string_value);
    }
    
    // Liberar todos los hijos recursivamente
    for (int i = 0; i < node->child_count; i++) {
        free_ast_node(node->children[i]);
    }
    
    // Liberar el array de hijos
    if (node->children) {
        free(node->children);
    }
    
    // Liberar el nodo
    free(node);
}

void print_ast(ASTNode* node, int depth) {
    if (!node) return;
    
    // Imprimir indentación
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    // Imprimir tipo de nodo
    printf("%s", get_node_type_name(node->type));
    
    // Imprimir valores si existen
    if (node->string_value) {
        printf(" (\"%s\")", node->string_value);
    }
    if (node->int_value != 0 || node->type == INTEGER_LITERAL_NODE || 
        node->type == BOOL_LITERAL_NODE) {
        printf(" (%d)", node->int_value);
    }
    
    printf("\n");
    
    // Imprimir hijos recursivamente
    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], depth + 1);
    }
}

const char* get_node_type_name(ASTNodeType type) {
    switch (type) {
        case PROGRAM_NODE: return "PROGRAM";
        case VAR_DECL_NODE: return "VAR_DECL";
        case VAR_DECL_LIST_NODE: return "VAR_DECL_LIST";
        case METHOD_DECL_LIST_NODE: return "METHOD_DECL_LIST";
        case METHOD_DECL_NODE: return "METHOD_DECL";
        case EXTERN_METHOD_DECL_NODE: return "EXTERN_METHOD_DECL";
        case PARAM_LIST_NODE: return "PARAM_LIST";
        case PARAM_NODE: return "PARAM";
        case BLOCK_NODE: return "BLOCK";
        case STATEMENT_LIST_NODE: return "STATEMENT_LIST";
        case ASSIGN_NODE: return "ASSIGN";
        case ASSIGN_STMT_NODE: return "ASSIGN_STMT";
        case METHOD_CALL_STMT_NODE: return "METHOD_CALL_STMT";
        case IF_NODE: return "IF";
        case IF_STMT_NODE: return "IF_STMT";
        case WHILE_NODE: return "WHILE";
        case WHILE_STMT_NODE: return "WHILE_STMT";
        case RETURN_NODE: return "RETURN";
        case RETURN_STMT_NODE: return "RETURN_STMT";
        case EMPTY_STMT_NODE: return "EMPTY_STMT";
        case IDENTIFIER_NODE: return "IDENTIFIER";
        case METHOD_CALL_NODE: return "METHOD_CALL";
        case BINARY_OP_NODE: return "BINARY_OP";
        case UNARY_OP_NODE: return "UNARY_OP";
        case ARITH_OP_NODE: return "ARITH_OP";
        case REL_OP_NODE: return "REL_OP";
        case COND_OP_NODE: return "COND_OP";
        case OPERATOR_NODE: return "OPERATOR";
        case INTEGER_LITERAL_NODE: return "INTEGER_LITERAL";
        case BOOLEAN_LITERAL_NODE: return "BOOLEAN_LITERAL";
        case BOOL_LITERAL_NODE: return "BOOL_LITERAL";
        case TYPE_NODE: return "TYPE";
        case INTEGER_TYPE_NODE: return "INTEGER_TYPE";
        case BOOL_TYPE_NODE: return "BOOL_TYPE";
        case VOID_TYPE_NODE: return "VOID_TYPE";
        case ARG_LIST_NODE: return "ARG_LIST";
        case EMPTY_NODE: return "EMPTY";
        default: return "UNKNOWN";
    }
}
