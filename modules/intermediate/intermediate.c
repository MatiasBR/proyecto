#define _GNU_SOURCE
#include "intermediate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

IRCode* generate_intermediate_code(ASTNode* root) {
    if (!root) {
        return NULL;
    }
    
    printf("Generando código intermedio...\n");
    
    IRCode* ir_code = malloc(sizeof(IRCode));
    if (!ir_code) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el código intermedio\n");
        return NULL;
    }
    
    ir_code->instructions = NULL;
    ir_code->temp_count = 0;
    
    // Generar código intermedio basado en el AST
    generate_program_ir(root, ir_code);
    
    return ir_code;
}

void print_ir_code(IRCode* ir_code) {
    if (!ir_code) {
        printf("Código intermedio vacío\n");
        return;
    }
    
    printf("=== Código Intermedio ===\n");
    IRInstruction* current = ir_code->instructions;
    while (current) {
        switch (current->type) {
            case IR_ASSIGN:
                printf("%s = %s\n", current->result, current->arg1);
                break;
            case IR_ADD:
                printf("%s = %s + %s\n", current->result, current->arg1, current->arg2);
                break;
            case IR_SUB:
                printf("%s = %s - %s\n", current->result, current->arg1, current->arg2);
                break;
            case IR_MUL:
                printf("%s = %s * %s\n", current->result, current->arg1, current->arg2);
                break;
            case IR_DIV:
                printf("%s = %s / %s\n", current->result, current->arg1, current->arg2);
                break;
            case IR_MOD:
                printf("%s = %s %% %s\n", current->result, current->arg1, current->arg2);
                break;
            case IR_LT:
                printf("%s = %s < %s\n", current->result, current->arg1, current->arg2);
                break;
            case IR_GT:
                printf("%s = %s > %s\n", current->result, current->arg1, current->arg2);
                break;
            case IR_EQ:
                printf("%s = %s == %s\n", current->result, current->arg1, current->arg2);
                break;
            case IR_AND:
                printf("%s = %s && %s\n", current->result, current->arg1, current->arg2);
                break;
            case IR_OR:
                printf("%s = %s || %s\n", current->result, current->arg1, current->arg2);
                break;
            case IR_NOT:
                printf("%s = !%s\n", current->result, current->arg1);
                break;
            case IR_NEG:
                printf("%s = -%s\n", current->result, current->arg1);
                break;
            case IR_LABEL:
                printf("%s:\n", current->label);
                break;
            case IR_GOTO:
                printf("goto %s\n", current->label);
                break;
            case IR_IF_GOTO:
                printf("if %s goto %s\n", current->arg1, current->label);
                break;
            case IR_CALL:
                printf("%s = call %s\n", current->result, current->arg1);
                break;
            case IR_RETURN:
                if (current->arg1) {
                    printf("return %s\n", current->arg1);
                } else {
                    printf("return\n");
                }
                break;
            case IR_PARAM:
                printf("param %s\n", current->arg1);
                break;
            case IR_FUNC_START:
                printf("function %s start\n", current->arg1);
                break;
            case IR_FUNC_END:
                printf("function %s end\n", current->arg1);
                break;
        }
        current = current->next;
    }
}

void free_ir_code(IRCode* ir_code) {
    if (!ir_code) return;
    
    IRInstruction* current = ir_code->instructions;
    while (current) {
        IRInstruction* next = current->next;
        if (current->result) free(current->result);
        if (current->arg1) free(current->arg1);
        if (current->arg2) free(current->arg2);
        if (current->label) free(current->label);
        free(current);
        current = next;
    }
    
    free(ir_code);
}

char* generate_temp() {
    static int temp_counter = 0;
    char* temp = malloc(16);
    snprintf(temp, 16, "t%d", temp_counter++);
    return temp;
}

IRInstruction* create_ir_instruction(IRInstructionType type, char* result, char* arg1, char* arg2, char* label) {
    IRInstruction* instr = malloc(sizeof(IRInstruction));
    if (!instr) {
        return NULL;
    }
    
    instr->type = type;
    instr->result = result ? strdup(result) : NULL;
    instr->arg1 = arg1 ? strdup(arg1) : NULL;
    instr->arg2 = arg2 ? strdup(arg2) : NULL;
    instr->label = label ? strdup(label) : NULL;
    instr->next = NULL;
    
    return instr;
}

void add_instruction(IRCode* ir_code, IRInstruction* instr) {
    if (!ir_code || !instr) return;
    
    if (!ir_code->instructions) {
        ir_code->instructions = instr;
    } else {
        IRInstruction* current = ir_code->instructions;
        while (current->next) {
            current = current->next;
        }
        current->next = instr;
    }
}

void generate_program_ir(ASTNode* node, IRCode* ir_code) {
    if (!node || node->type != PROGRAM_NODE) {
        return;
    }
    
    // Generar código IR para cada declaración de variable
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child) {
            switch (child->type) {
                case VAR_DECL_NODE:
                    generate_var_decl_ir(child, ir_code);
                    break;
                case METHOD_DECL_NODE:
                    generate_method_decl_ir(child, ir_code);
                    break;
                case BLOCK_NODE:
                    generate_block_ir(child, ir_code);
                    break;
                default:
                    break;
            }
        }
    }
}

void generate_var_decl_ir(ASTNode* node, IRCode* ir_code) {
    if (!node || node->type != VAR_DECL_NODE) {
        return;
    }
    
    // Para declaraciones de variables, generar inicialización
    if (node->child_count >= 3) {
        ASTNode* id_node = node->children[1];  // identificador
        ASTNode* expr_node = node->children[2]; // expresión
        
        if (id_node && expr_node) {
            char* var_name = id_node->string_value;
            char* temp = generate_expression_ir(expr_node, ir_code);
            
            if (temp && var_name) {
                IRInstruction* instr = create_ir_instruction(IR_ASSIGN, var_name, temp, NULL, NULL);
                if (instr) {
                    add_instruction(ir_code, instr);
                }
            }
        }
    }
}

void generate_method_decl_ir(ASTNode* node, IRCode* ir_code) {
    if (!node || node->type != METHOD_DECL_NODE) {
        return;
    }
    
    // Generar etiqueta de inicio de función
    if (node->child_count >= 1) {
        ASTNode* id_node = node->children[0];
        if (id_node && id_node->string_value) {
            char* func_name = id_node->string_value;
            IRInstruction* start_instr = create_ir_instruction(IR_FUNC_START, NULL, func_name, NULL, NULL);
            if (start_instr) {
                add_instruction(ir_code, start_instr);
            }
            
            // Generar código para el cuerpo de la función
            if (node->child_count >= 2) {
                ASTNode* block_node = node->children[1];
                if (block_node) {
                    generate_block_ir(block_node, ir_code);
                }
            }
            
            // Generar etiqueta de fin de función
            IRInstruction* end_instr = create_ir_instruction(IR_FUNC_END, NULL, func_name, NULL, NULL);
            if (end_instr) {
                add_instruction(ir_code, end_instr);
            }
        }
    }
}

void generate_block_ir(ASTNode* node, IRCode* ir_code) {
    if (!node || node->type != BLOCK_NODE) {
        return;
    }
    
    // Generar código IR para cada declaración y sentencia en el bloque
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child) {
            switch (child->type) {
                case VAR_DECL_NODE:
                    generate_var_decl_ir(child, ir_code);
                    break;
                case ASSIGN_STMT_NODE:
                    generate_assign_stmt_ir(child, ir_code);
                    break;
                case IF_STMT_NODE:
                    generate_if_stmt_ir(child, ir_code);
                    break;
                case WHILE_STMT_NODE:
                    generate_while_stmt_ir(child, ir_code);
                    break;
                case RETURN_STMT_NODE:
                    generate_return_stmt_ir(child, ir_code);
                    break;
                case METHOD_CALL_STMT_NODE:
                    generate_method_call_stmt_ir(child, ir_code);
                    break;
                default:
                    break;
            }
        }
    }
}

void generate_assign_stmt_ir(ASTNode* node, IRCode* ir_code) {
    if (!node || node->type != ASSIGN_STMT_NODE) {
        return;
    }
    
    if (node->child_count >= 2) {
        ASTNode* id_node = node->children[0];    // variable
        ASTNode* expr_node = node->children[1];  // expresión
        
        if (id_node && expr_node) {
            char* var_name = id_node->string_value;
            char* temp = generate_expression_ir(expr_node, ir_code);
            
            if (temp && var_name) {
                IRInstruction* instr = create_ir_instruction(IR_ASSIGN, var_name, temp, NULL, NULL);
                if (instr) {
                    add_instruction(ir_code, instr);
                }
            }
        }
    }
}

void generate_if_stmt_ir(ASTNode* node, IRCode* ir_code) {
    if (!node || node->type != IF_STMT_NODE) {
        return;
    }
    
    if (node->child_count >= 3) {
        ASTNode* expr_node = node->children[0];     // condición
        ASTNode* then_block = node->children[1];    // bloque then
        ASTNode* else_block = node->children[2];    // bloque else
        
        char* condition_temp = generate_expression_ir(expr_node, ir_code);
        
        if (condition_temp) {
            char* else_label = generate_label();
            char* end_label = generate_label();
            
            // Generar salto condicional al else
            IRInstruction* if_goto = create_ir_instruction(IR_IF_GOTO, NULL, condition_temp, NULL, else_label);
            if (if_goto) {
                add_instruction(ir_code, if_goto);
            }
            
            // Generar código del bloque then
            if (then_block) {
                generate_block_ir(then_block, ir_code);
            }
            
            // Salto al final
            IRInstruction* goto_end = create_ir_instruction(IR_GOTO, NULL, NULL, NULL, end_label);
            if (goto_end) {
                add_instruction(ir_code, goto_end);
            }
            
            // Etiqueta del else
            IRInstruction* else_label_instr = create_ir_instruction(IR_LABEL, NULL, NULL, NULL, else_label);
            if (else_label_instr) {
                add_instruction(ir_code, else_label_instr);
            }
            
            // Generar código del bloque else
            if (else_block) {
                generate_block_ir(else_block, ir_code);
            }
            
            // Etiqueta del final
            IRInstruction* end_label_instr = create_ir_instruction(IR_LABEL, NULL, NULL, NULL, end_label);
            if (end_label_instr) {
                add_instruction(ir_code, end_label_instr);
            }
        }
    }
}

void generate_while_stmt_ir(ASTNode* node, IRCode* ir_code) {
    if (!node || node->type != WHILE_STMT_NODE) {
        return;
    }
    
    if (node->child_count >= 2) {
        ASTNode* expr_node = node->children[0];  // condición
        ASTNode* block_node = node->children[1]; // bloque
        
        char* start_label = generate_label();
        char* end_label = generate_label();
        
        // Etiqueta de inicio del while
        IRInstruction* start_label_instr = create_ir_instruction(IR_LABEL, NULL, NULL, NULL, start_label);
        if (start_label_instr) {
            add_instruction(ir_code, start_label_instr);
        }
        
        char* condition_temp = generate_expression_ir(expr_node, ir_code);
        
        if (condition_temp) {
            // Salto condicional al final si la condición es falsa
            IRInstruction* if_goto = create_ir_instruction(IR_IF_GOTO, NULL, condition_temp, NULL, end_label);
            if (if_goto) {
                add_instruction(ir_code, if_goto);
            }
            
            // Generar código del bloque
            if (block_node) {
                generate_block_ir(block_node, ir_code);
            }
            
            // Salto de vuelta al inicio
            IRInstruction* goto_start = create_ir_instruction(IR_GOTO, NULL, NULL, NULL, start_label);
            if (goto_start) {
                add_instruction(ir_code, goto_start);
            }
            
            // Etiqueta del final
            IRInstruction* end_label_instr = create_ir_instruction(IR_LABEL, NULL, NULL, NULL, end_label);
            if (end_label_instr) {
                add_instruction(ir_code, end_label_instr);
            }
        }
    }
}

void generate_return_stmt_ir(ASTNode* node, IRCode* ir_code) {
    if (!node || node->type != RETURN_STMT_NODE) {
        return;
    }
    
    if (node->child_count >= 1) {
        ASTNode* expr_node = node->children[0];
        if (expr_node) {
            char* temp = generate_expression_ir(expr_node, ir_code);
            IRInstruction* instr = create_ir_instruction(IR_RETURN, NULL, temp, NULL, NULL);
            if (instr) {
                add_instruction(ir_code, instr);
            }
        }
    } else {
        // Return sin expresión
        IRInstruction* instr = create_ir_instruction(IR_RETURN, NULL, NULL, NULL, NULL);
        if (instr) {
            add_instruction(ir_code, instr);
        }
    }
}

void generate_method_call_stmt_ir(ASTNode* node, IRCode* ir_code) {
    if (!node || node->type != METHOD_CALL_STMT_NODE) {
        return;
    }
    
    if (node->child_count >= 1) {
        ASTNode* method_call = node->children[0];
        if (method_call) {
            generate_method_call_ir(method_call, ir_code);
        }
    }
}

void generate_method_call_ir(ASTNode* node, IRCode* ir_code) {
    if (!node || node->type != METHOD_CALL_NODE) {
        return;
    }
    
    if (node->child_count >= 1) {
        ASTNode* id_node = node->children[0];
        if (id_node && id_node->string_value) {
            char* method_name = id_node->string_value;
            
            // Generar parámetros
            if (node->child_count >= 2) {
                ASTNode* arg_list = node->children[1];
                if (arg_list) {
                    generate_arg_list_ir(arg_list, ir_code);
                }
            }
            
            // Generar llamada
            char* result_temp = generate_temp();
            IRInstruction* call_instr = create_ir_instruction(IR_CALL, result_temp, method_name, NULL, NULL);
            if (call_instr) {
                add_instruction(ir_code, call_instr);
            }
        }
    }
}

void generate_arg_list_ir(ASTNode* node, IRCode* ir_code) {
    if (!node || node->type != ARG_LIST_NODE) {
        return;
    }
    
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child) {
            char* temp = generate_expression_ir(child, ir_code);
            if (temp) {
                IRInstruction* param_instr = create_ir_instruction(IR_PARAM, NULL, temp, NULL, NULL);
                if (param_instr) {
                    add_instruction(ir_code, param_instr);
                }
            }
        }
    }
}

char* generate_expression_ir(ASTNode* node, IRCode* ir_code) {
    if (!node) {
        return NULL;
    }
    
    switch (node->type) {
        case INTEGER_LITERAL_NODE:
        case BOOLEAN_LITERAL_NODE:
        case BOOL_LITERAL_NODE: {
            char* temp = generate_temp();
            char* value = malloc(16);
            if (value) {
                snprintf(value, 16, "%d", node->int_value);
                IRInstruction* instr = create_ir_instruction(IR_ASSIGN, temp, value, NULL, NULL);
                if (instr) {
                    add_instruction(ir_code, instr);
                }
                free(value);
            }
            return temp;
        }
        
        case IDENTIFIER_NODE: {
            return strdup(node->string_value);
        }
        
        case BINARY_OP_NODE:
        case ARITH_OP_NODE:
        case REL_OP_NODE:
        case COND_OP_NODE: {
            if (node->child_count >= 3) {
                ASTNode* left = node->children[0];
                ASTNode* op = node->children[1];
                ASTNode* right = node->children[2];
                
                char* left_temp = generate_expression_ir(left, ir_code);
                char* right_temp = generate_expression_ir(right, ir_code);
                
                if (left_temp && right_temp && op) {
                    char* result_temp = generate_temp();
                    IRInstructionType ir_type = get_ir_type_from_operator(op->string_value);
                    
                    IRInstruction* instr = create_ir_instruction(ir_type, result_temp, left_temp, right_temp, NULL);
                    if (instr) {
                        add_instruction(ir_code, instr);
                    }
                    
                    free(left_temp);
                    free(right_temp);
                    return result_temp;
                }
            }
            break;
        }
        
        case UNARY_OP_NODE: {
            if (node->child_count >= 2) {
                ASTNode* op = node->children[0];
                ASTNode* operand = node->children[1];
                
                char* operand_temp = generate_expression_ir(operand, ir_code);
                
                if (operand_temp && op) {
                    char* result_temp = generate_temp();
                    IRInstructionType ir_type = get_ir_type_from_unary_operator(op->string_value);
                    
                    IRInstruction* instr = create_ir_instruction(ir_type, result_temp, operand_temp, NULL, NULL);
                    if (instr) {
                        add_instruction(ir_code, instr);
                    }
                    
                    free(operand_temp);
                    return result_temp;
                }
            }
            break;
        }
        
        case METHOD_CALL_NODE: {
            generate_method_call_ir(node, ir_code);
            return generate_temp(); // Retornar temporal para el resultado
        }
        
        default:
            break;
    }
    
    return NULL;
}

IRInstructionType get_ir_type_from_operator(char* op) {
    if (!op) return IR_ASSIGN;
    
    if (strcmp(op, "+") == 0) return IR_ADD;
    if (strcmp(op, "-") == 0) return IR_SUB;
    if (strcmp(op, "*") == 0) return IR_MUL;
    if (strcmp(op, "/") == 0) return IR_DIV;
    if (strcmp(op, "%") == 0) return IR_MOD;
    if (strcmp(op, "<") == 0) return IR_LT;
    if (strcmp(op, ">") == 0) return IR_GT;
    if (strcmp(op, "==") == 0) return IR_EQ;
    if (strcmp(op, "&&") == 0) return IR_AND;
    if (strcmp(op, "||") == 0) return IR_OR;
    
    return IR_ASSIGN;
}

IRInstructionType get_ir_type_from_unary_operator(char* op) {
    if (!op) return IR_ASSIGN;
    
    if (strcmp(op, "-") == 0) return IR_NEG;
    if (strcmp(op, "!") == 0) return IR_NOT;
    
    return IR_ASSIGN;
}

char* generate_label() {
    static int label_counter = 0;
    char* label = malloc(16);
    if (label) {
        snprintf(label, 16, "L%d", label_counter++);
    }
    return label;
}
