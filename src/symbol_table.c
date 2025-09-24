#include "symbol_table.h"

SymbolTable* create_symbol_table(SymbolTable* parent) {
    SymbolTable* table = malloc(sizeof(SymbolTable));
    if (!table) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la tabla de símbolos\n");
        exit(1);
    }
    
    table->symbols = NULL;
    table->scope_level = parent ? parent->scope_level + 1 : 0;
    table->parent = parent;
    table->children = NULL;
    table->next_sibling = NULL;
    
    return table;
}

void free_symbol_table(SymbolTable* table) {
    if (!table) return;
    
    // Liberar todos los símbolos
    Symbol* current = table->symbols;
    while (current) {
        Symbol* next = current->next;
        if (current->name) free(current->name);
        free(current);
        current = next;
    }
    
    // Liberar tablas hijas recursivamente
    SymbolTable* child = table->children;
    while (child) {
        SymbolTable* next_child = child->next_sibling;
        free_symbol_table(child);
        child = next_child;
    }
    
    free(table);
}

Symbol* create_symbol(char* name, SymbolType type, DataType data_type, int scope_level) {
    Symbol* symbol = malloc(sizeof(Symbol));
    if (!symbol) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el símbolo\n");
        exit(1);
    }
    
    symbol->name = strdup(name);
    symbol->symbol_type = type;
    symbol->data_type = data_type;
    symbol->scope_level = scope_level;
    symbol->is_initialized = 0;
    symbol->is_extern = 0;
    symbol->next = NULL;
    
    return symbol;
}

void add_symbol(SymbolTable* table, Symbol* symbol) {
    if (!table || !symbol) return;
    
    // Verificar si ya existe un símbolo con el mismo nombre en el mismo scope
    Symbol* existing = lookup_symbol_in_scope(table, symbol->name);
    if (existing) {
        fprintf(stderr, "Error semántico: El símbolo '%s' ya está definido en este scope\n", symbol->name);
        return;
    }
    
    // Agregar al inicio de la lista
    symbol->next = table->symbols;
    table->symbols = symbol;
}

Symbol* lookup_symbol(SymbolTable* table, char* name) {
    if (!table || !name) return NULL;
    
    // Buscar en el scope actual
    Symbol* symbol = lookup_symbol_in_scope(table, name);
    if (symbol) return symbol;
    
    // Buscar en scopes padre
    SymbolTable* parent = table->parent;
    while (parent) {
        symbol = lookup_symbol_in_scope(parent, name);
        if (symbol) return symbol;
        parent = parent->parent;
    }
    
    return NULL;
}

Symbol* lookup_symbol_in_scope(SymbolTable* table, char* name) {
    if (!table || !name) return NULL;
    
    Symbol* current = table->symbols;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

void enter_scope(SymbolTable** table) {
    SymbolTable* new_scope = create_symbol_table(*table);
    
    if (*table) {
        // Agregar como hijo del scope actual
        new_scope->next_sibling = (*table)->children;
        (*table)->children = new_scope;
    }
    
    *table = new_scope;
}

void exit_scope(SymbolTable** table) {
    if (*table && (*table)->parent) {
        *table = (*table)->parent;
    }
}

void print_symbol_table(SymbolTable* table, int depth) {
    if (!table) return;
    
    // Imprimir indentación
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    printf("Scope level %d:\n", table->scope_level);
    
    // Imprimir símbolos en este scope
    Symbol* current = table->symbols;
    while (current) {
        for (int i = 0; i < depth + 1; i++) {
            printf("  ");
        }
        printf("%s (%s, %s, level %d)\n", 
               current->name,
               get_symbol_type_name(current->symbol_type),
               get_data_type_name(current->data_type),
               current->scope_level);
        current = current->next;
    }
    
    // Imprimir scopes hijos
    SymbolTable* child = table->children;
    while (child) {
        print_symbol_table(child, depth + 1);
        child = child->next_sibling;
    }
}

const char* get_symbol_type_name(SymbolType type) {
    switch (type) {
        case SYMBOL_VARIABLE: return "VARIABLE";
        case SYMBOL_FUNCTION: return "FUNCTION";
        case SYMBOL_PARAMETER: return "PARAMETER";
        default: return "UNKNOWN";
    }
}

const char* get_data_type_name(DataType type) {
    switch (type) {
        case TYPE_INTEGER: return "INTEGER";
        case TYPE_BOOL: return "BOOL";
        case TYPE_VOID: return "VOID";
        default: return "UNKNOWN";
    }
}
