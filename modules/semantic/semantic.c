#include "semantic.h"

static int semantic_errors = 0;

void semantic_error(const char* message, int line) {
    fprintf(stderr, "Error semántico en línea %d: %s\n", line, message);
    semantic_errors++;
}

int semantic_analysis(ASTNode* root) {
    if (!root) {
        semantic_error("Árbol AST vacío", 0);
        return 0;
    }
    
    semantic_errors = 0;
    SymbolTable* global_table = create_symbol_table(NULL);
    
    int result = analyze_program(root, global_table);
    
    if (semantic_errors == 0) {
        printf("Análisis semántico completado exitosamente\n");
    } else {
        printf("Análisis semántico completado con %d errores\n", semantic_errors);
    }
    
    free_symbol_table(global_table);
    return (semantic_errors == 0) ? 1 : 0;
}

int analyze_program(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != PROGRAM_NODE) {
        semantic_error("Nodo de programa inválido", 0);
        return 0;
    }
    
    // Analizar declaraciones de variables globales
    if (node->children[0]) {
        analyze_var_decl_list(node->children[0], table);
    }
    
    // Analizar declaraciones de métodos
    if (node->children[1]) {
        analyze_method_decl_list(node->children[1], table);
    }
    
    // Verificar que existe el método main
    Symbol* main_func = lookup_symbol(table, "main");
    if (!main_func) {
        semantic_error("El programa debe contener un método 'main'", 0);
        return 0;
    }
    
    if (main_func->symbol_type != SYMBOL_FUNCTION) {
        semantic_error("'main' debe ser una función", 0);
        return 0;
    }
    
    if (main_func->data_type != TYPE_VOID) {
        semantic_error("El método 'main' debe ser de tipo void", 0);
        return 0;
    }
    
    return 1;
}

int analyze_var_decl_list(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != VAR_DECL_LIST_NODE) return 1;
    
    for (int i = 0; i < node->child_count; i++) {
        analyze_var_decl(node->children[i], table);
    }
    
    return 1;
}

int analyze_method_decl_list(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != METHOD_DECL_LIST_NODE) return 1;
    
    for (int i = 0; i < node->child_count; i++) {
        analyze_method_decl(node->children[i], table);
    }
    
    return 1;
}

int analyze_var_decl(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != VAR_DECL_NODE) return 1;
    
    if (!node->string_value) {
        semantic_error("Variable sin nombre", 0);
        return 0;
    }
    
    // Obtener tipo de la variable
    DataType var_type = get_expr_type(node->children[0], table);
    
    // Crear símbolo para la variable
    Symbol* var_symbol = create_symbol(node->string_value, SYMBOL_VARIABLE, var_type, table->scope_level);
    var_symbol->is_initialized = 1;
    add_symbol(table, var_symbol);
    
    // Analizar expresión de inicialización
    DataType expr_type;
    if (!analyze_expr(node->children[1], table, &expr_type)) {
        return 0;
    }
    
    // Verificar compatibilidad de tipos
    if (!type_compatible(var_type, expr_type)) {
        semantic_error("Tipo incompatible en inicialización de variable", 0);
        return 0;
    }
    
    return 1;
}

int analyze_method_decl(ASTNode* node, SymbolTable* table) {
    if (!node || (node->type != METHOD_DECL_NODE && node->type != EXTERN_METHOD_DECL_NODE)) {
        return 1;
    }
    
    if (!node->string_value) {
        semantic_error("Método sin nombre", 0);
        return 0;
    }
    
    // Obtener tipo de retorno
    DataType return_type = get_expr_type(node->children[0], table);
    
    // Crear símbolo para la función
    Symbol* func_symbol = create_symbol(node->string_value, SYMBOL_FUNCTION, return_type, table->scope_level);
    if (node->type == EXTERN_METHOD_DECL_NODE) {
        func_symbol->is_extern = 1;
    }
    add_symbol(table, func_symbol);
    
    // Si no es externa, analizar el cuerpo
    if (node->type == METHOD_DECL_NODE && node->children[2]) {
        // Crear nuevo scope para la función
        SymbolTable* func_table = create_symbol_table(table);
        func_table->scope_level = 1;
        
        // Analizar parámetros
        if (node->children[1]) {
            analyze_param_list(node->children[1], func_table);
        }
        
        // Analizar bloque de la función
        analyze_block(node->children[2], func_table);
        
        free_symbol_table(func_table);
    }
    
    return 1;
}

int analyze_param_list(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != PARAM_LIST_NODE) return 1;
    
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* param = node->children[i];
        if (param && param->type == PARAM_NODE && param->string_value) {
            DataType param_type = get_expr_type(param->children[0], table);
            Symbol* param_symbol = create_symbol(param->string_value, SYMBOL_PARAMETER, param_type, table->scope_level);
            add_symbol(table, param_symbol);
        }
    }
    
    return 1;
}

int analyze_block(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != BLOCK_NODE) return 1;
    
    // Crear nuevo scope para el bloque
    SymbolTable* block_table = create_symbol_table(table);
    
    // Analizar declaraciones de variables locales
    if (node->children[0]) {
        analyze_var_decl_list(node->children[0], block_table);
    }
    
    // Analizar statements
    if (node->children[1]) {
        analyze_statement_list(node->children[1], block_table);
    }
    
    free_symbol_table(block_table);
    return 1;
}

int analyze_statement_list(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != STATEMENT_LIST_NODE) return 1;
    
    for (int i = 0; i < node->child_count; i++) {
        analyze_statement(node->children[i], table);
    }
    
    return 1;
}

int analyze_statement(ASTNode* node, SymbolTable* table) {
    if (!node) return 1;
    
    switch (node->type) {
        case ASSIGN_STMT_NODE: {
            if (!node->string_value) {
                semantic_error("Asignación sin variable", 0);
                return 0;
            }
            
            Symbol* var = lookup_symbol(table, node->string_value);
            if (!var) {
                semantic_error("Variable no declarada", 0);
                return 0;
            }
            
            DataType expr_type;
            if (!analyze_expr(node->children[0], table, &expr_type)) {
                return 0;
            }
            
            if (!type_compatible(var->data_type, expr_type)) {
                semantic_error("Tipo incompatible en asignación", 0);
                return 0;
            }
            break;
        }
        
        case METHOD_CALL_STMT_NODE: {
            DataType result_type;
            analyze_method_call(node->children[0], table, &result_type);
            break;
        }
        
        case IF_STMT_NODE: {
            DataType cond_type;
            if (!analyze_expr(node->children[0], table, &cond_type)) {
                return 0;
            }
            
            if (cond_type != TYPE_BOOL) {
                semantic_error("Condición del if debe ser booleana", 0);
                return 0;
            }
            
            analyze_block(node->children[1], table);
            if (node->children[2]) {
                analyze_block(node->children[2], table);
            }
            break;
        }
        
        case WHILE_STMT_NODE: {
            DataType cond_type;
            if (!analyze_expr(node->children[0], table, &cond_type)) {
                return 0;
            }
            
            if (cond_type != TYPE_BOOL) {
                semantic_error("Condición del while debe ser booleana", 0);
                return 0;
            }
            
            analyze_block(node->children[1], table);
            break;
        }
        
        case RETURN_STMT_NODE: {
            if (node->children[0] && node->children[0]->type != EMPTY_NODE) {
                DataType expr_type;
                analyze_expr(node->children[0], table, &expr_type);
            }
            break;
        }
        
        case BLOCK_NODE:
            analyze_block(node, table);
            break;
            
        default:
            break;
    }
    
    return 1;
}

int analyze_expr(ASTNode* node, SymbolTable* table, DataType* result_type) {
    if (!node) {
        *result_type = TYPE_VOID;
        return 1;
    }
    
    switch (node->type) {
        case IDENTIFIER_NODE: {
            if (!node->string_value) {
                semantic_error("Identificador sin nombre", 0);
                *result_type = TYPE_VOID;
                return 0;
            }
            
            Symbol* symbol = lookup_symbol(table, node->string_value);
            if (!symbol) {
                semantic_error("Identificador no declarado", 0);
                *result_type = TYPE_VOID;
                return 0;
            }
            
            *result_type = symbol->data_type;
            return 1;
        }
        
        case METHOD_CALL_NODE:
            return analyze_method_call(node, table, result_type);
            
        case INTEGER_LITERAL_NODE:
            *result_type = TYPE_INTEGER;
            return 1;
            
        case BOOL_LITERAL_NODE:
            *result_type = TYPE_BOOL;
            return 1;
            
        case BINARY_OP_NODE: {
            DataType left_type, right_type;
            if (!analyze_expr(node->children[0], table, &left_type) ||
                !analyze_expr(node->children[2], table, &right_type)) {
                *result_type = TYPE_VOID;
                return 0;
            }
            
            // Verificar compatibilidad de tipos
            if (!type_compatible(left_type, right_type)) {
                semantic_error("Tipos incompatibles en operación binaria", 0);
                *result_type = TYPE_VOID;
                return 0;
            }
            
            *result_type = left_type;
            return 1;
        }
        
        case UNARY_OP_NODE: {
            DataType expr_type;
            if (!analyze_expr(node->children[0], table, &expr_type)) {
                *result_type = TYPE_VOID;
                return 0;
            }
            
            *result_type = expr_type;
            return 1;
        }
        
        default:
            *result_type = TYPE_VOID;
            return 1;
    }
}

int analyze_method_call(ASTNode* node, SymbolTable* table, DataType* result_type) {
    if (!node || node->type != METHOD_CALL_NODE) {
        *result_type = TYPE_VOID;
        return 1;
    }
    
    if (!node->string_value) {
        semantic_error("Llamada a método sin nombre", 0);
        *result_type = TYPE_VOID;
        return 0;
    }
    
    Symbol* func = lookup_symbol(table, node->string_value);
    if (!func) {
        semantic_error("Función no declarada", 0);
        *result_type = TYPE_VOID;
        return 0;
    }
    
    if (func->symbol_type != SYMBOL_FUNCTION) {
        semantic_error("No es una función", 0);
        *result_type = TYPE_VOID;
        return 0;
    }
    
    *result_type = func->data_type;
    return 1;
}

DataType get_expr_type(ASTNode* node, SymbolTable* table) {
    if (!node) return TYPE_VOID;
    
    switch (node->type) {
        case INTEGER_TYPE_NODE: return TYPE_INTEGER;
        case BOOL_TYPE_NODE: return TYPE_BOOL;
        case VOID_TYPE_NODE: return TYPE_VOID;
        default: return TYPE_VOID;
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
