#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER
} SymbolType;

typedef struct Symbol {
    char *name;
    char *type;
    SymbolType symbol_type;
    int scope_level;
    int is_initialized;
    struct Symbol *next;
} Symbol;

typedef struct SymbolTable {
    struct SymbolTable *parent;
    Symbol *symbols;
    int num_symbols;
    struct SymbolTable **children;
    int num_children;
    char *function_name;
    int scope_level;
} SymbolTable;

extern SymbolTable *current_table;
extern SymbolTable *global_table;

// Funciones principales
void init_symtab();
void push_scope();
void pop_scope();
void push_scope_for_function(char *function_name);
Symbol* search_symbol(char *name);
Symbol* search_symbol_in_scope(char *name, SymbolTable *scope);
void insert_symbol(char *name, char *type, SymbolType symbol_type);
void print_symtab();
void print_scope_recursive(SymbolTable *table, int depth);
void debug_print_scopes();
void free_symtab();
int get_current_scope_level();

// Funciones de validación semántica
int check_variable_declared(char *name);
int check_function_declared(char *name);
int check_type_compatibility(char *type1, char *type2);
int check_assignment_compatibility(char *var_type, char *expr_type);

#endif