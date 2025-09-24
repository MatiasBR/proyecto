#define _GNU_SOURCE
#include "assembly.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int stack_offset = 0;
static int label_count = 0;

int generate_assembly_code(IRCode* ir_code, const char* output_filename) {
    FILE* file = fopen(output_filename, "w");
    if (!file) {
        fprintf(stderr, "Error: No se pudo crear el archivo assembly %s\n", output_filename);
        return 0;
    }
    
    // Generar encabezado del archivo assembly
    fprintf(file, "; Código assembly generado por TDS25 compiler\n");
    fprintf(file, ".section .text\n");
    fprintf(file, ".global main\n");
    fprintf(file, "\n");
    
    // Generar prólogo
    generate_prologue(file);
    
    // Generar instrucciones IR
    IRInstruction* current = ir_code->instructions;
    while (current) {
        generate_instruction(file, current);
        current = current->next;
    }
    
    // Generar epílogo
    generate_epilogue(file);
    
    fclose(file);
    return 1;
}

void generate_prologue(FILE* file) {
    fprintf(file, "main:\n");
    fprintf(file, "    push rbp\n");
    fprintf(file, "    mov rbp, rsp\n");
    fprintf(file, "    sub rsp, 32    ; Espacio para variables locales\n");
    fprintf(file, "\n");
}

void generate_epilogue(FILE* file) {
    fprintf(file, "\n");
    fprintf(file, "    mov rsp, rbp\n");
    fprintf(file, "    pop rbp\n");
    fprintf(file, "    mov rax, 0     ; Código de salida\n");
    fprintf(file, "    ret\n");
}

void generate_instruction(FILE* file, IRInstruction* instruction) {
    switch (instruction->type) {
        case IR_ASSIGN:
            fprintf(file, "    ; Asignación: %s = %s\n", 
                    instruction->result, instruction->arg1);
            if (instruction->arg1 && instruction->result) {
                fprintf(file, "    mov %s, %s\n", 
                        get_register_for_temp(instruction->result),
                        get_register_for_temp(instruction->arg1));
            }
            break;
            
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
        case IR_MOD:
            generate_arithmetic(file, instruction);
            break;
            
        case IR_LT:
        case IR_GT:
        case IR_EQ:
            generate_comparison(file, instruction);
            break;
            
        case IR_AND:
        case IR_OR:
        case IR_NOT:
            generate_logical(file, instruction);
            break;
            
        case IR_LABEL:
            fprintf(file, "%s:\n", instruction->label);
            break;
            
        case IR_GOTO:
            fprintf(file, "    jmp %s\n", instruction->label);
            break;
            
        case IR_IF_GOTO:
            fprintf(file, "    cmp %s, 0\n", get_register_for_temp(instruction->arg1));
            fprintf(file, "    jne %s\n", instruction->label);
            break;
            
        case IR_CALL:
            generate_function_call(file, instruction);
            break;
            
        case IR_RETURN:
            if (instruction->arg1) {
                fprintf(file, "    mov rax, %s\n", get_register_for_temp(instruction->arg1));
            }
            fprintf(file, "    ret\n");
            break;
            
        case IR_PARAM:
            fprintf(file, "    push %s\n", get_register_for_temp(instruction->arg1));
            break;
            
        case IR_FUNC_START:
            fprintf(file, "%s:\n", instruction->arg1);
            fprintf(file, "    push rbp\n");
            fprintf(file, "    mov rbp, rsp\n");
            break;
            
        case IR_FUNC_END:
            fprintf(file, "    pop rbp\n");
            fprintf(file, "    ret\n");
            break;
            
        default:
            fprintf(file, "    ; Instrucción no implementada: %d\n", instruction->type);
            break;
    }
}

void generate_arithmetic(FILE* file, IRInstruction* instruction) {
    const char* op = get_assembly_operator(instruction->type);
    fprintf(file, "    ; Operación aritmética: %s = %s %s %s\n",
            instruction->result, instruction->arg1, op, instruction->arg2);
    
    if (instruction->arg1 && instruction->arg2 && instruction->result) {
        fprintf(file, "    mov rax, %s\n", get_register_for_temp(instruction->arg1));
        fprintf(file, "    %s rax, %s\n", op, get_register_for_temp(instruction->arg2));
        fprintf(file, "    mov %s, rax\n", get_register_for_temp(instruction->result));
    }
}

void generate_comparison(FILE* file, IRInstruction* instruction) {
    const char* op = get_assembly_operator(instruction->type);
    fprintf(file, "    ; Comparación: %s = %s %s %s\n",
            instruction->result, instruction->arg1, op, instruction->arg2);
    
    if (instruction->arg1 && instruction->arg2 && instruction->result) {
        fprintf(file, "    mov rax, %s\n", get_register_for_temp(instruction->arg1));
        fprintf(file, "    cmp rax, %s\n", get_register_for_temp(instruction->arg2));
        fprintf(file, "    set%s al\n", op);
        fprintf(file, "    movzx %s, al\n", get_register_for_temp(instruction->result));
    }
}

void generate_logical(FILE* file, IRInstruction* instruction) {
    fprintf(file, "    ; Operación lógica: %s = %s %s %s\n",
            instruction->result, instruction->arg1, 
            instruction->type == IR_NOT ? "!" : get_assembly_operator(instruction->type),
            instruction->arg2 ? instruction->arg2 : "");
    
    if (instruction->type == IR_NOT) {
        fprintf(file, "    mov rax, %s\n", get_register_for_temp(instruction->arg1));
        fprintf(file, "    not rax\n");
        fprintf(file, "    mov %s, rax\n", get_register_for_temp(instruction->result));
    } else {
        fprintf(file, "    mov rax, %s\n", get_register_for_temp(instruction->arg1));
        fprintf(file, "    %s rax, %s\n", get_assembly_operator(instruction->type), 
                get_register_for_temp(instruction->arg2));
        fprintf(file, "    mov %s, rax\n", get_register_for_temp(instruction->result));
    }
}

void generate_control_flow(FILE* file, IRInstruction* instruction) {
    switch (instruction->type) {
        case IR_LABEL:
            fprintf(file, "%s:\n", instruction->label);
            break;
        case IR_GOTO:
            fprintf(file, "    jmp %s\n", instruction->label);
            break;
        case IR_IF_GOTO:
            fprintf(file, "    cmp %s, 0\n", get_register_for_temp(instruction->arg1));
            fprintf(file, "    jne %s\n", instruction->label);
            break;
    }
}

void generate_function_call(FILE* file, IRInstruction* instruction) {
    fprintf(file, "    ; Llamada a función: %s = call %s\n",
            instruction->result, instruction->arg1);
    fprintf(file, "    call %s\n", instruction->arg1);
    if (instruction->result) {
        fprintf(file, "    mov %s, rax\n", get_register_for_temp(instruction->result));
    }
}

const char* get_register_for_temp(const char* temp) {
    // Mapeo simple de temporales a registros
    // En una implementación real, esto sería más sofisticado
    static char register_name[20];
    if (temp && temp[0] == 't') {
        int num = atoi(temp + 1);
        switch (num % 4) {
            case 0: return "rax";
            case 1: return "rbx";
            case 2: return "rcx";
            case 3: return "rdx";
            default: return "rax";
        }
    }
    return "rax";
}

const char* get_assembly_operator(IRType type) {
    switch (type) {
        case IR_ADD: return "add";
        case IR_SUB: return "sub";
        case IR_MUL: return "imul";
        case IR_DIV: return "idiv";
        case IR_MOD: return "mod";
        case IR_LT: return "l";
        case IR_GT: return "g";
        case IR_EQ: return "e";
        case IR_AND: return "and";
        case IR_OR: return "or";
        case IR_NOT: return "not";
        default: return "mov";
    }
}

void allocate_stack_space(FILE* file, int bytes) {
    fprintf(file, "    sub rsp, %d\n", bytes);
    stack_offset += bytes;
}

void deallocate_stack_space(FILE* file, int bytes) {
    fprintf(file, "    add rsp, %d\n", bytes);
    stack_offset -= bytes;
}
