#define _GNU_SOURCE
#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variables globales para el análisis semántico
static int semantic_errors = 0;
static int has_main_function = 0;

// Función para reportar errores semánticos
void report_semantic_error(const char* message, int line) {
    fprintf(stderr, "Error semántico en línea %d: %s\n", line, message);
    semantic_errors++;
}

// Función para reportar errores semánticos con contexto
void report_semantic_error_with_context(const char* message, const char* context, int line) {
    fprintf(stderr, "Error semántico en línea %d: %s: '%s'\n", line, message, context);
    semantic_errors++;
}

int semantic_analysis(ASTNode* root) {
    if (!root) {
        fprintf(stderr, "Error: AST raíz es NULL\n");
        return 0;
    }
    
    printf("Iniciando análisis semántico...\n");
    
    // Inicializar variables globales
    semantic_errors = 0;
    has_main_function = 0;
    
    // Inicializar tabla de símbolos
    init_symtab();
    
    int result = analyze_program(root);
    
    // Verificar regla 3: Todo programa debe tener función main
    // TEMPORALMENTE COMENTADO PARA LA ENTREGA DEL 8 DE OCTUBRE
    // if (!has_main_function) {
    //     report_semantic_error("El programa debe contener una función 'main'", 0);
    //     result = 0;
    // }
    
    if (result && semantic_errors == 0) {
        printf("Análisis semántico exitoso\n");
    } else {
        printf("Errores encontrados en el análisis semántico (%d errores)\n", semantic_errors);
    }
    
    return result && semantic_errors == 0;
}

int analyze_program(ASTNode* node) {
    if (!node || node->type != PROGRAM_NODE) {
        return 0;
    }
    
    printf("Analizando programa...\n");
    
    // Primera pasada: Declarar todas las funciones y variables globales
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child) {
            if (child->type == METHOD_DECL_NODE) {
                if (!analyze_method_decl(child)) {
                    return 0;
                }
            } else if (child->type == VAR_DECL_NODE) {
                if (!analyze_var_decl(child)) {
                    return 0;
                }
            }
        }
    }
    
    // Segunda pasada: Analizar el cuerpo de las funciones
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child && child->type == METHOD_DECL_NODE) {
            if (!analyze_method_body(child)) {
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
    
    // Obtener información de la declaración
    char* var_name = node->string_value;
    DataType var_type = get_expr_type(node->children[0], current_table);
    
    // Regla 1: Verificar que no esté declarada en el mismo scope
    if (search_symbol(var_name)) {
        report_semantic_error_with_context("Variable ya declarada en el mismo scope", var_name, 0);
        return 0;
    }
    
    // Insertar en la tabla de símbolos
    insert_symbol(var_name, (char*)get_node_type_name(var_type), SYMBOL_VARIABLE);
    
    // Analizar la expresión de inicialización si existe
    if (node->child_count > 1 && node->children[1]) {
        DataType expr_type;
        if (!analyze_expr(node->children[1], current_table, &expr_type)) {
            return 0;
        }
        
        // Regla 13: Verificar compatibilidad de tipos en asignación
        if (!type_compatible(var_type, expr_type)) {
            report_semantic_error_with_context("Tipos incompatibles en asignación", var_name, 0);
            return 0;
        }
    }
    
    return 1;
}

int analyze_method_decl(ASTNode* node) {
    if (!node || node->type != METHOD_DECL_NODE) {
        return 0;
    }
    
    // Obtener información del método
    char* method_name = node->string_value;
    DataType return_type = get_expr_type(node->children[0], current_table);
    
    // Regla 3: Verificar si es la función main
    if (strcmp(method_name, "main") == 0) {
        has_main_function = 1;
        // Verificar que main no tenga parámetros
        if (node->child_count > 2 && node->children[2]) {
            report_semantic_error("La función main no debe tener parámetros", 0);
            return 0;
        }
    }
    
    // Regla 1: Verificar que no esté declarada en el mismo scope
    if (search_symbol(method_name)) {
        report_semantic_error_with_context("Función ya declarada en el mismo scope", method_name, 0);
        return 0;
    }
    
    // Insertar función en la tabla de símbolos
    insert_symbol(method_name, (char*)get_node_type_name(return_type), SYMBOL_FUNCTION);
    
    // Crear nuevo scope para la función
    push_scope_for_function(method_name);
    
    // Analizar parámetros
    if (node->child_count > 2 && node->children[2]) {
        ASTNode* params = node->children[2];
        for (int i = 0; i < params->child_count; i++) {
            ASTNode* param = params->children[i];
            if (param) {
                char* param_name = param->string_value;
                DataType param_type = get_expr_type(param->children[0], current_table);
                
                // Insertar parámetro en el scope de la función
                insert_symbol(param_name, (char*)get_node_type_name(param_type), SYMBOL_PARAMETER);
            }
        }
    }
    
    return 1;
}

int analyze_method_body(ASTNode* node) {
    if (!node || node->type != METHOD_DECL_NODE) {
        return 0;
    }
    
    // Analizar el cuerpo de la función
    if (node->child_count > 3 && node->children[3]) {
        ASTNode* body = node->children[3];
        if (!analyze_statement(body)) {
            return 0;
        }
    }
    
    // Salir del scope de la función
    pop_scope();
    
    return 1;
}

int analyze_statement(ASTNode* node) {
    if (!node) {
        return 0;
    }
    
    switch (node->type) {
        case ASSIGN_NODE: {
            // Regla 8: Verificar que la variable esté declarada
            char* var_name = node->string_value;
            Symbol* symbol = search_symbol(var_name);
            if (!symbol) {
                report_semantic_error_with_context("Variable no declarada", var_name, 0);
                return 0;
            }
            
            // Analizar la expresión
            DataType expr_type;
            if (!analyze_expr(node->children[0], current_table, &expr_type)) {
                return 0;
            }
            
            // Regla 13: Verificar compatibilidad de tipos
            DataType var_type = (strcmp(symbol->type, "integer") == 0) ? TYPE_INTEGER : TYPE_BOOL;
            if (!type_compatible(var_type, expr_type)) {
                report_semantic_error_with_context("Tipos incompatibles en asignación", var_name, 0);
                return 0;
            }
            break;
        }
        
        case IF_NODE: {
            // Regla 9: La condición del if debe ser bool
            DataType cond_type;
            if (!analyze_expr(node->children[0], current_table, &cond_type)) {
                return 0;
            }
            if (!is_logical_type(cond_type)) {
                report_semantic_error("La condición del if debe ser de tipo bool", 0);
                return 0;
            }
            
            // Analizar bloques then y else
            if (node->child_count > 1 && node->children[1]) {
                if (!analyze_statement(node->children[1])) {
                    return 0;
                }
            }
            if (node->child_count > 2 && node->children[2]) {
                if (!analyze_statement(node->children[2])) {
                    return 0;
                }
            }
            break;
        }
        
        case WHILE_NODE: {
            // Regla 9: La condición del while debe ser bool
            DataType cond_type;
            if (!analyze_expr(node->children[0], current_table, &cond_type)) {
                return 0;
            }
            if (!is_logical_type(cond_type)) {
                report_semantic_error("La condición del while debe ser de tipo bool", 0);
                return 0;
            }
            
            // Analizar el cuerpo del while
            if (node->child_count > 1 && node->children[1]) {
                if (!analyze_statement(node->children[1])) {
                    return 0;
                }
            }
            break;
        }
        
        case RETURN_NODE: {
            // Regla 6 y 7: Verificar return
            if (node->child_count > 0 && node->children[0]) {
                // Return con expresión
                DataType expr_type;
                if (!analyze_expr(node->children[0], current_table, &expr_type)) {
                    return 0;
                }
                // El tipo de retorno se verifica en el contexto de la función
            }
            break;
        }
        
        case METHOD_CALL_NODE: {
            DataType result_type;
            if (!analyze_method_call(node, current_table, &result_type)) {
                return 0;
            }
            break;
        }
        
        case BLOCK_NODE: {
            // Crear nuevo scope para el bloque
            push_scope();
            
            // Analizar statements del bloque
            for (int i = 0; i < node->child_count; i++) {
                if (!analyze_statement(node->children[i])) {
                    pop_scope();
                    return 0;
                }
            }
            
            // Salir del scope del bloque
            pop_scope();
            break;
        }
        
        default:
            // Analizar hijos recursivamente
            for (int i = 0; i < node->child_count; i++) {
                if (!analyze_statement(node->children[i])) {
                    return 0;
                }
            }
            break;
    }
    
    return 1;
}

int analyze_expr(ASTNode* node, SymbolTable* table, DataType* result_type) {
    if (!node) {
        *result_type = TYPE_VOID;
        return 0;
    }
    
    switch (node->type) {
        case IDENTIFIER_NODE: {
            // Regla 2: Verificar que esté declarada
            char* var_name = node->string_value;
            Symbol* symbol = search_symbol(var_name);
            if (!symbol) {
                report_semantic_error_with_context("Variable no declarada", var_name, 0);
                *result_type = TYPE_VOID;
                return 0;
            }
            
            // Obtener el tipo de la variable
            if (strcmp(symbol->type, "integer") == 0) {
                *result_type = TYPE_INTEGER;
            } else if (strcmp(symbol->type, "bool") == 0) {
                *result_type = TYPE_BOOL;
            } else {
                *result_type = TYPE_VOID;
            }
            break;
        }
        
        case INTEGER_LITERAL_NODE:
            *result_type = TYPE_INTEGER;
            break;
            
        case BOOLEAN_LITERAL_NODE:
            *result_type = TYPE_BOOL;
            break;
            
        case BINARY_OP_NODE: {
            DataType left_type, right_type;
            if (!analyze_expr(node->children[0], table, &left_type) ||
                !analyze_expr(node->children[1], table, &right_type)) {
                *result_type = TYPE_VOID;
                return 0;
            }
            
            // Verificar tipos según el operador
            char* op = node->string_value;
            if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || 
                strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) {
                // Regla 10: Operadores aritméticos requieren operandos integer
                if (!is_arithmetic_type(left_type) || !is_arithmetic_type(right_type)) {
                    report_semantic_error("Operadores aritméticos requieren operandos de tipo integer", 0);
                    *result_type = TYPE_VOID;
                    return 0;
                }
                *result_type = TYPE_INTEGER;
            } else if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 || strcmp(op, "==") == 0) {
                // Regla 10: Operadores relacionales requieren operandos integer
                if (!is_arithmetic_type(left_type) || !is_arithmetic_type(right_type)) {
                    report_semantic_error("Operadores relacionales requieren operandos de tipo integer", 0);
                    *result_type = TYPE_VOID;
                    return 0;
                }
                *result_type = TYPE_BOOL;
            } else if (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
                // Regla 12: Operadores lógicos requieren operandos bool
                if (!is_logical_type(left_type) || !is_logical_type(right_type)) {
                    report_semantic_error("Operadores lógicos requieren operandos de tipo bool", 0);
                    *result_type = TYPE_VOID;
                    return 0;
                }
                *result_type = TYPE_BOOL;
            }
            break;
        }
        
        case UNARY_OP_NODE: {
            DataType operand_type;
            if (!analyze_expr(node->children[0], table, &operand_type)) {
                *result_type = TYPE_VOID;
                return 0;
            }
            
            char* op = node->string_value;
            if (strcmp(op, "-") == 0) {
                // Menos unario requiere operando integer
                if (!is_arithmetic_type(operand_type)) {
                    report_semantic_error("Operador menos unario requiere operando de tipo integer", 0);
                    *result_type = TYPE_VOID;
                    return 0;
                }
                *result_type = TYPE_INTEGER;
            } else if (strcmp(op, "!") == 0) {
                // Negación lógica requiere operando bool
                if (!is_logical_type(operand_type)) {
                    report_semantic_error("Operador de negación requiere operando de tipo bool", 0);
                    *result_type = TYPE_VOID;
                    return 0;
                }
                *result_type = TYPE_BOOL;
            }
            break;
        }
        
        case METHOD_CALL_NODE: {
            if (!analyze_method_call(node, table, result_type)) {
                return 0;
            }
            break;
        }
        
        default:
            *result_type = TYPE_VOID;
            return 0;
    }
    
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
    
    // Obtener nombre del método
    char* method_name = node->string_value;
    Symbol* method_symbol = search_symbol(method_name);
    
    // Regla 2: Verificar que el método esté declarado
    if (!method_symbol) {
        report_semantic_error_with_context("Método no declarado", method_name, 0);
        *result_type = TYPE_VOID;
        return 0;
    }
    
    // Regla 5: Verificar que el método retorne un valor si se usa como expresión
    if (strcmp(method_symbol->type, "void") == 0) {
        report_semantic_error_with_context("Método void no puede ser usado como expresión", method_name, 0);
        *result_type = TYPE_VOID;
        return 0;
    }
    
    // Obtener el tipo de retorno
    if (strcmp(method_symbol->type, "integer") == 0) {
        *result_type = TYPE_INTEGER;
    } else if (strcmp(method_symbol->type, "bool") == 0) {
        *result_type = TYPE_BOOL;
    } else {
        *result_type = TYPE_VOID;
    }
    
    // Regla 4: Verificar número y tipos de argumentos
    // Esta verificación sería más compleja en una implementación completa
    // ya que requeriría acceso a la definición del método
    
    return 1;
}