#define _GNU_SOURCE
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variables globales
SymbolTable *current_table = NULL;
SymbolTable *global_table = NULL;

void init_symtab() {
    global_table = malloc(sizeof(SymbolTable));
    if (!global_table) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la tabla global\n");
        exit(1);
    }
    
    global_table->parent = NULL;
    global_table->symbols = NULL;
    global_table->num_symbols = 0;
    global_table->children = NULL;
    global_table->num_children = 0;
    global_table->function_name = NULL;
    global_table->scope_level = 0;
    
    current_table = global_table;
}

void push_scope() {
    SymbolTable *new_scope = malloc(sizeof(SymbolTable));
    if (!new_scope) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el nuevo scope\n");
        exit(1);
    }
    
    new_scope->parent = current_table;
    new_scope->symbols = NULL;
    new_scope->num_symbols = 0;
    new_scope->children = NULL;
    new_scope->num_children = 0;
    new_scope->function_name = NULL;
    new_scope->scope_level = current_table ? current_table->scope_level + 1 : 0;
    
    current_table = new_scope;
}

void pop_scope() {
    if (current_table && current_table->parent) {
        SymbolTable *parent = current_table->parent;
        free_symtab();
        current_table = parent;
    }
}

void push_scope_for_function(char *function_name) {
    push_scope();
    if (current_table) {
        current_table->function_name = strdup(function_name);
    }
}

Symbol* search_symbol(char *name) {
    SymbolTable *table = current_table;
    
    while (table) {
        Symbol *current = table->symbols;
        while (current) {
            if (strcmp(current->name, name) == 0) {
                return current;
            }
            current = current->next;
        }
        table = table->parent;
    }
    
    return NULL;
}

Symbol* search_symbol_in_scope(char *name, SymbolTable *scope) {
    if (!scope) return NULL;
    
    Symbol *current = scope->symbols;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

void insert_symbol(char *name, char *type, SymbolType symbol_type) {
    Symbol *new_symbol = malloc(sizeof(Symbol));
    if (!new_symbol) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el símbolo\n");
        exit(1);
    }
    
    new_symbol->name = strdup(name);
    new_symbol->type = strdup(type);
    new_symbol->symbol_type = symbol_type;
    new_symbol->scope_level = current_table ? current_table->scope_level : 0;
    new_symbol->is_initialized = 0;
    new_symbol->next = NULL;
    
    // Agregar al inicio de la lista
    new_symbol->next = current_table->symbols;
    current_table->symbols = new_symbol;
    current_table->num_symbols++;
}

void print_symtab() {
    printf("=== Tabla de Símbolos ===\n");
    print_scope_recursive(current_table, 0);
}

void print_scope_recursive(SymbolTable *table, int depth) {
    if (!table) return;
    
    for (int i = 0; i < depth; i++) printf("  ");
    printf("Scope level %d", table->scope_level);
    if (table->function_name) {
        printf(" (function: %s)", table->function_name);
    }
    printf("\n");
    
    Symbol *current = table->symbols;
    while (current) {
        for (int i = 0; i < depth + 1; i++) printf("  ");
        printf("- %s (%s, type: %d, level: %d)\n", 
               current->name, current->type, current->symbol_type, current->scope_level);
        current = current->next;
    }
    
    // Imprimir scopes hijos
    for (int i = 0; i < table->num_children; i++) {
        print_scope_recursive(table->children[i], depth + 1);
    }
}

void debug_print_scopes() {
    printf("=== Debug: Scopes ===\n");
    SymbolTable *table = current_table;
    int level = 0;
    
    while (table) {
        printf("Level %d: scope_level=%d, function=%s\n", 
               level, table->scope_level, 
               table->function_name ? table->function_name : "global");
        table = table->parent;
        level++;
    }
}

void free_symtab() {
    if (!current_table) return;
    
    // Liberar símbolos
    Symbol *current = current_table->symbols;
    while (current) {
        Symbol *next = current->next;
        if (current->name) free(current->name);
        if (current->type) free(current->type);
        free(current);
        current = next;
    }
    
    // Liberar scopes hijos
    for (int i = 0; i < current_table->num_children; i++) {
        free_symtab();
    }
    
    if (current_table->function_name) {
        free(current_table->function_name);
    }
    
    free(current_table);
    current_table = NULL;
}

int get_current_scope_level() {
    return current_table ? current_table->scope_level : 0;
}

// Funciones de validación semántica
int check_variable_declared(char *name) {
    Symbol *symbol = search_symbol(name);
    return symbol && symbol->symbol_type == SYMBOL_VARIABLE;
}

int check_function_declared(char *name) {
    Symbol *symbol = search_symbol(name);
    return symbol && symbol->symbol_type == SYMBOL_FUNCTION;
}

int check_type_compatibility(char *type1, char *type2) {
    return strcmp(type1, type2) == 0;
}

int check_assignment_compatibility(char *var_type, char *expr_type) {
    return strcmp(var_type, expr_type) == 0;
}