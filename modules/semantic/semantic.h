#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../ast/ast.h"
#include "../common/symbol_table.h"

// Function declarations
int semantic_analysis(ASTNode* root);
int analyze_program(ASTNode* node, SymbolTable* table);
int analyze_var_decl_list(ASTNode* node, SymbolTable* table);
int analyze_method_decl_list(ASTNode* node, SymbolTable* table);
int analyze_var_decl(ASTNode* node, SymbolTable* table);
int analyze_method_decl(ASTNode* node, SymbolTable* table);
int analyze_param_list(ASTNode* node, SymbolTable* table);
int analyze_block(ASTNode* node, SymbolTable* table);
int analyze_statement_list(ASTNode* node, SymbolTable* table);
int analyze_statement(ASTNode* node, SymbolTable* table);
int analyze_expr(ASTNode* node, SymbolTable* table, DataType* result_type);
int analyze_method_call(ASTNode* node, SymbolTable* table, DataType* result_type);

// Type checking functions
DataType get_expr_type(ASTNode* node, SymbolTable* table);
int type_compatible(DataType type1, DataType type2);
int is_arithmetic_type(DataType type);
int is_logical_type(DataType type);

// Utility functions
const char* get_data_type_name(DataType type);
void semantic_error(const char* message, int line);

#endif
