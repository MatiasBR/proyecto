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
    
    // Crear una instrucción de ejemplo
    IRInstruction* instr = create_ir_instruction(IR_ASSIGN, "t1", "5", NULL, NULL);
    if (instr) {
        ir_code->instructions = instr;
    }
    
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