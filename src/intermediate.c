#include "intermediate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int temp_counter = 0;
static int label_counter = 0;

char* new_temp() {
    char* temp = malloc(20);
    sprintf(temp, "t%d", temp_counter++);
    return temp;
}

char* new_label() {
    char* label = malloc(20);
    sprintf(label, "L%d", label_counter++);
    return label;
}

IRInstruction* create_ir_instruction(IRType type, char* result, char* arg1, char* arg2, char* label) {
    IRInstruction* instruction = malloc(sizeof(IRInstruction));
    if (!instruction) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la instrucción IR\n");
        exit(1);
    }
    
    instruction->type = type;
    instruction->result = result ? strdup(result) : NULL;
    instruction->arg1 = arg1 ? strdup(arg1) : NULL;
    instruction->arg2 = arg2 ? strdup(arg2) : NULL;
    instruction->label = label ? strdup(label) : NULL;
    instruction->next = NULL;
    
    return instruction;
}

void add_ir_instruction(IRCode* code, IRInstruction* instruction) {
    if (!code || !instruction) return;
    
    if (!code->instructions) {
        code->instructions = instruction;
    } else {
        IRInstruction* current = code->instructions;
        while (current->next) {
            current = current->next;
        }
        current->next = instruction;
    }
    code->instruction_count++;
}

void print_ir_code(IRCode* code) {
    if (!code || !code->instructions) return;
    
    IRInstruction* current = code->instructions;
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

void free_ir_code(IRCode* code) {
    if (!code) return;
    
    IRInstruction* current = code->instructions;
    while (current) {
        IRInstruction* next = current->next;
        free_ir_instruction(current);
        current = next;
    }
    
    free(code);
}

void free_ir_instruction(IRInstruction* instruction) {
    if (!instruction) return;
    
    if (instruction->result) free(instruction->result);
    if (instruction->arg1) free(instruction->arg1);
    if (instruction->arg2) free(instruction->arg2);
    if (instruction->label) free(instruction->label);
    
    free(instruction);
}

IRCode* generate_intermediate_code(ASTNode* root) {
    if (!root) return NULL;
    
    IRCode* code = malloc(sizeof(IRCode));
    if (!code) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el código IR\n");
        exit(1);
    }
    
    code->instructions = NULL;
    code->instruction_count = 0;
    
    // Generar código para el programa
    IRCode* program_code = generate_program_code(root);
    if (program_code) {
        add_ir_instruction(code, program_code->instructions);
        free(program_code);
    }
    
    return code;
}

IRCode* generate_program_code(ASTNode* node) {
    if (!node || node->type != PROGRAM_NODE) return NULL;
    
    IRCode* code = malloc(sizeof(IRCode));
    code->instructions = NULL;
    code->instruction_count = 0;
    
    // Generar código para métodos
    if (node->children[1]) { // method_decl_list
        for (int i = 0; i < node->children[1]->child_count; i++) {
            IRCode* method_code = generate_method_code(node->children[1]->children[i]);
            if (method_code) {
                add_ir_instruction(code, method_code->instructions);
                free(method_code);
            }
        }
    }
    
    return code;
}

IRCode* generate_method_code(ASTNode* node) {
    if (!node || (node->type != METHOD_DECL_NODE && node->type != EXTERN_METHOD_DECL_NODE)) {
        return NULL;
    }
    
    IRCode* code = malloc(sizeof(IRCode));
    code->instructions = NULL;
    code->instruction_count = 0;
    
    // Función start
    IRInstruction* func_start = create_ir_instruction(IR_FUNC_START, NULL, node->string_value, NULL, NULL);
    add_ir_instruction(code, func_start);
    
    // Si no es externa, generar código del bloque
    if (node->type == METHOD_DECL_NODE && node->children[2]) {
        IRCode* block_code = generate_block_code(node->children[2]);
        if (block_code) {
            add_ir_instruction(code, block_code->instructions);
            free(block_code);
        }
    }
    
    // Función end
    IRInstruction* func_end = create_ir_instruction(IR_FUNC_END, NULL, node->string_value, NULL, NULL);
    add_ir_instruction(code, func_end);
    
    return code;
}

IRCode* generate_block_code(ASTNode* node) {
    if (!node || node->type != BLOCK_NODE) return NULL;
    
    IRCode* code = malloc(sizeof(IRCode));
    code->instructions = NULL;
    code->instruction_count = 0;
    
    // Generar código para statements
    if (node->children[1]) { // statement_list
        for (int i = 0; i < node->children[1]->child_count; i++) {
            IRCode* stmt_code = generate_statement_code(node->children[1]->children[i]);
            if (stmt_code) {
                add_ir_instruction(code, stmt_code->instructions);
                free(stmt_code);
            }
        }
    }
    
    return code;
}

IRCode* generate_statement_code(ASTNode* node) {
    if (!node) return NULL;
    
    IRCode* code = malloc(sizeof(IRCode));
    code->instructions = NULL;
    code->instruction_count = 0;
    
    switch (node->type) {
        case ASSIGN_STMT_NODE: {
            IRCode* expr_code = generate_expr_code(node->children[0]);
            if (expr_code) {
                add_ir_instruction(code, expr_code->instructions);
                free(expr_code);
            }
            
            // Asignación
            IRInstruction* assign = create_ir_instruction(IR_ASSIGN, node->string_value, 
                                                         node->children[0]->string_value, NULL, NULL);
            add_ir_instruction(code, assign);
            break;
        }
        
        case METHOD_CALL_STMT_NODE: {
            IRCode* call_code = generate_expr_code(node->children[0]);
            if (call_code) {
                add_ir_instruction(code, call_code->instructions);
                free(call_code);
            }
            break;
        }
        
        case IF_STMT_NODE: {
            char* else_label = new_label();
            char* end_label = new_label();
            
            // Generar código para la condición
            IRCode* cond_code = generate_expr_code(node->children[0]);
            if (cond_code) {
                add_ir_instruction(code, cond_code->instructions);
                free(cond_code);
            }
            
            // if-goto
            IRInstruction* if_goto = create_ir_instruction(IR_IF_GOTO, NULL, 
                                                          node->children[0]->string_value, NULL, else_label);
            add_ir_instruction(code, if_goto);
            
            // Código del then
            IRCode* then_code = generate_block_code(node->children[1]);
            if (then_code) {
                add_ir_instruction(code, then_code->instructions);
                free(then_code);
            }
            
            // goto end
            IRInstruction* goto_end = create_ir_instruction(IR_GOTO, NULL, NULL, NULL, end_label);
            add_ir_instruction(code, goto_end);
            
            // else label
            IRInstruction* else_lbl = create_ir_instruction(IR_LABEL, NULL, NULL, NULL, else_label);
            add_ir_instruction(code, else_lbl);
            
            // Código del else
            if (node->children[2]) {
                IRCode* else_code = generate_block_code(node->children[2]);
                if (else_code) {
                    add_ir_instruction(code, else_code->instructions);
                    free(else_code);
                }
            }
            
            // end label
            IRInstruction* end_lbl = create_ir_instruction(IR_LABEL, NULL, NULL, NULL, end_label);
            add_ir_instruction(code, end_lbl);
            
            free(else_label);
            free(end_label);
            break;
        }
        
        case WHILE_STMT_NODE: {
            char* loop_label = new_label();
            char* end_label = new_label();
            
            // loop label
            IRInstruction* loop_lbl = create_ir_instruction(IR_LABEL, NULL, NULL, NULL, loop_label);
            add_ir_instruction(code, loop_lbl);
            
            // Generar código para la condición
            IRCode* cond_code = generate_expr_code(node->children[0]);
            if (cond_code) {
                add_ir_instruction(code, cond_code->instructions);
                free(cond_code);
            }
            
            // if-goto end
            IRInstruction* if_goto = create_ir_instruction(IR_IF_GOTO, NULL, 
                                                          node->children[0]->string_value, NULL, end_label);
            add_ir_instruction(code, if_goto);
            
            // Código del cuerpo
            IRCode* body_code = generate_block_code(node->children[1]);
            if (body_code) {
                add_ir_instruction(code, body_code->instructions);
                free(body_code);
            }
            
            // goto loop
            IRInstruction* goto_loop = create_ir_instruction(IR_GOTO, NULL, NULL, NULL, loop_label);
            add_ir_instruction(code, goto_loop);
            
            // end label
            IRInstruction* end_lbl = create_ir_instruction(IR_LABEL, NULL, NULL, NULL, end_label);
            add_ir_instruction(code, end_lbl);
            
            free(loop_label);
            free(end_label);
            break;
        }
        
        case RETURN_STMT_NODE: {
            if (node->children[0] && node->children[0]->type != EMPTY_NODE) {
                IRCode* expr_code = generate_expr_code(node->children[0]);
                if (expr_code) {
                    add_ir_instruction(code, expr_code->instructions);
                    free(expr_code);
                }
                
                IRInstruction* ret = create_ir_instruction(IR_RETURN, NULL, 
                                                        node->children[0]->string_value, NULL, NULL);
                add_ir_instruction(code, ret);
            } else {
                IRInstruction* ret = create_ir_instruction(IR_RETURN, NULL, NULL, NULL, NULL);
                add_ir_instruction(code, ret);
            }
            break;
        }
        
        case BLOCK_NODE:
            return generate_block_code(node);
            
        default:
            break;
    }
    
    return code;
}

IRCode* generate_expr_code(ASTNode* node) {
    if (!node) return NULL;
    
    IRCode* code = malloc(sizeof(IRCode));
    code->instructions = NULL;
    code->instruction_count = 0;
    
    switch (node->type) {
        case IDENTIFIER_NODE:
        case INTEGER_LITERAL_NODE:
        case BOOL_LITERAL_NODE:
            // Los literales e identificadores no generan código adicional
            break;
            
        case BINARY_OP_NODE: {
            IRCode* left_code = generate_expr_code(node->children[0]);
            IRCode* right_code = generate_expr_code(node->children[2]);
            
            if (left_code) {
                add_ir_instruction(code, left_code->instructions);
                free(left_code);
            }
            if (right_code) {
                add_ir_instruction(code, right_code->instructions);
                free(right_code);
            }
            
            // Determinar operación
            IRType op_type;
            char* op = node->children[1]->string_value;
            if (strcmp(op, "+") == 0) op_type = IR_ADD;
            else if (strcmp(op, "-") == 0) op_type = IR_SUB;
            else if (strcmp(op, "*") == 0) op_type = IR_MUL;
            else if (strcmp(op, "/") == 0) op_type = IR_DIV;
            else if (strcmp(op, "%") == 0) op_type = IR_MOD;
            else if (strcmp(op, "<") == 0) op_type = IR_LT;
            else if (strcmp(op, ">") == 0) op_type = IR_GT;
            else if (strcmp(op, "==") == 0) op_type = IR_EQ;
            else if (strcmp(op, "&&") == 0) op_type = IR_AND;
            else if (strcmp(op, "||") == 0) op_type = IR_OR;
            else op_type = IR_ASSIGN;
            
            char* temp = new_temp();
            IRInstruction* instruction = create_ir_instruction(op_type, temp, 
                                                              node->children[0]->string_value,
                                                              node->children[2]->string_value, NULL);
            add_ir_instruction(code, instruction);
            
            // Actualizar el string_value del nodo con el resultado
            if (node->string_value) free(node->string_value);
            node->string_value = strdup(temp);
            free(temp);
            break;
        }
        
        case UNARY_OP_NODE: {
            IRCode* expr_code = generate_expr_code(node->children[0]);
            if (expr_code) {
                add_ir_instruction(code, expr_code->instructions);
                free(expr_code);
            }
            
            char* temp = new_temp();
            IRType op_type = (strcmp(node->string_value, "-") == 0) ? IR_NEG : IR_NOT;
            IRInstruction* instruction = create_ir_instruction(op_type, temp, 
                                                            node->children[0]->string_value, NULL, NULL);
            add_ir_instruction(code, instruction);
            
            if (node->string_value) free(node->string_value);
            node->string_value = strdup(temp);
            free(temp);
            break;
        }
        
        case METHOD_CALL_NODE: {
            char* temp = new_temp();
            IRInstruction* call = create_ir_instruction(IR_CALL, temp, node->string_value, NULL, NULL);
            add_ir_instruction(code, call);
            
            if (node->string_value) free(node->string_value);
            node->string_value = strdup(temp);
            free(temp);
            break;
        }
        
        default:
            break;
    }
    
    return code;
}
