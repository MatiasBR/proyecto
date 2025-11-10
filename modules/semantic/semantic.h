#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../ast/ast.h"
#include "../common/symbol_table.h"

// Tipos de datos
typedef enum {
    TYPE_INTEGER,
    TYPE_BOOL,
    TYPE_VOID
} DataType;

// Funciones principales
int semantic_analysis(ASTNode* root);
int analyze_program(ASTNode* node);
int analyze_var_decl(ASTNode* node);
int analyze_method_decl(ASTNode* node);
int analyze_statement(ASTNode* node);
int analyze_expr(ASTNode* node, SymbolTable* table, DataType* result_type);

// Funciones de validación de tipos
DataType get_expr_type(ASTNode* node, SymbolTable* table);
int type_compatible(DataType type1, DataType type2);
int is_arithmetic_type(DataType type);
int is_logical_type(DataType type);

// Funciones auxiliares
int analyze_method_call(ASTNode* node, SymbolTable* table, DataType* result_type);
int analyze_method_body(ASTNode* node);

// Funciones de reporte de errores
void report_semantic_error(const char* message, int line);
void report_semantic_error_with_context(const char* message, const char* context, int line);

#endif