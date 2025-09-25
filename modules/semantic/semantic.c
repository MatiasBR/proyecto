#define _GNU_SOURCE
#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int semantic_analysis(ASTNode* root) {
    if (!root) {
        fprintf(stderr, "Error: AST raíz es NULL\n");
        return 0;
    }
    
    printf("Iniciando análisis semántico...\n");
    
    // Inicializar tabla de símbolos
    init_symtab();
    
    int result = analyze_program(root);
    
    if (result) {
        printf("Análisis semántico exitoso\n");
    } else {
        printf("Errores encontrados en el análisis semántico\n");
    }
    
    return result;
}

int analyze_program(ASTNode* node) {
    if (!node || node->type != PROGRAM_NODE) {
        return 0;
    }
    
    printf("Analizando programa...\n");
    
    // Analizar declaraciones de variables globales
    if (node->child_count > 0 && node->children[0]) {
        ASTNode* var_decl_list = node->children[0];
        for (int i = 0; i < var_decl_list->child_count; i++) {
            if (!analyze_var_decl(var_decl_list->children[i])) {
                return 0;
            }
        }
    }
    
    // Analizar declaraciones de métodos
    if (node->child_count > 1 && node->children[1]) {
        ASTNode* method_decl_list = node->children[1];
        for (int i = 0; i < method_decl_list->child_count; i++) {
            if (!analyze_method_decl(method_decl_list->children[i])) {
                return 0;
            }
        }
    }
    
    return 1;
}

int analyze_var_decl(ASTNode* node) {
    if (!node || node->type != VAR_DECL_NODE) {
        return 0;
    }
    
    printf("Analizando declaración de variable...\n");
    return 1;
}

int analyze_method_decl(ASTNode* node) {
    if (!node || node->type != METHOD_DECL_NODE) {
        return 0;
    }
    
    printf("Analizando declaración de método...\n");
    return 1;
}

int analyze_statement(ASTNode* node) {
    if (!node) {
        return 0;
    }
    
    printf("Analizando statement...\n");
    return 1;
}

int analyze_expr(ASTNode* node, SymbolTable* table, DataType* result_type) {
    if (!node) {
        *result_type = TYPE_VOID;
        return 0;
    }
    
    printf("Analizando expresión...\n");
    *result_type = TYPE_INTEGER; // Simplificado
    return 1;
}

DataType get_expr_type(ASTNode* node, SymbolTable* table) {
    if (!node) return TYPE_VOID;
    
    switch (node->type) {
        case INTEGER_TYPE_NODE: return TYPE_INTEGER;
        case BOOL_TYPE_NODE: return TYPE_BOOL;
        case INTEGER_LITERAL_NODE: return TYPE_INTEGER;
        case BOOLEAN_LITERAL_NODE: return TYPE_BOOL;
        default: return TYPE_INTEGER;
    }
}

int type_compatible(DataType type1, DataType type2) {
    return type1 == type2;
}

int is_arithmetic_type(DataType type) {
    return type == TYPE_INTEGER;
}

int is_logical_type(DataType type) {
    return type == TYPE_BOOL;
}

int analyze_method_call(ASTNode* node, SymbolTable* table, DataType* result_type) {
    if (!node || node->type != METHOD_CALL_NODE) {
        return 0;
    }
    
    printf("Analizando llamada a método...\n");
    *result_type = TYPE_INTEGER; // Simplificado
    return 1;
}