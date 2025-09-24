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

typedef enum {
    TYPE_INTEGER,
    TYPE_BOOL,
    TYPE_VOID
} DataType;

typedef struct Symbol {
    char* name;
    SymbolType symbol_type;
    DataType data_type;
    int scope_level;
    int is_initialized;
    int is_extern;
    struct Symbol* next;
} Symbol;

typedef struct SymbolTable {
    Symbol* symbols;
    int scope_level;
    struct SymbolTable* parent;
    struct SymbolTable* children;
    struct SymbolTable* next_sibling;
} SymbolTable;

// Function declarations
SymbolTable* create_symbol_table(SymbolTable* parent);
void free_symbol_table(SymbolTable* table);
Symbol* create_symbol(char* name, SymbolType type, DataType data_type, int scope_level);
void add_symbol(SymbolTable* table, Symbol* symbol);
Symbol* lookup_symbol(SymbolTable* table, char* name);
Symbol* lookup_symbol_in_scope(SymbolTable* table, char* name);
void enter_scope(SymbolTable** table);
void exit_scope(SymbolTable** table);
void print_symbol_table(SymbolTable* table, int depth);
const char* get_symbol_type_name(SymbolType type);
const char* get_data_type_name(DataType type);

#endif
