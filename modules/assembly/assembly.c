#define _GNU_SOURCE
#include "assembly.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int generate_assembly_code(IRCode* ir_code, const char* output_filename) {
    if (!ir_code || !output_filename) {
        return 0;
    }
    
    printf("Generando código assembly...\n");
    
    FILE* file = fopen(output_filename, "w");
    if (!file) {
        fprintf(stderr, "Error: No se pudo abrir el archivo %s para escritura\n", output_filename);
        return 0;
    }
    
    write_assembly_header(file);
    
    IRInstruction* current = ir_code->instructions;
    while (current) {
        write_instruction_assembly(file, current);
        current = current->next;
    }
    
    write_assembly_footer(file);
    
    fclose(file);
    printf("Código assembly generado en: %s\n", output_filename);
    
    return 1;
}

void write_assembly_header(FILE* file) {
    fprintf(file, ".section .text\n");
    fprintf(file, ".global _start\n");
    fprintf(file, ".global main\n");
    fprintf(file, "_start:\n");
    fprintf(file, "    call main\n");
    fprintf(file, "    mov $60, %%rax\n");
    fprintf(file, "    mov $0, %%rdi\n");
    fprintf(file, "    syscall\n");
    fprintf(file, "\n");
    fprintf(file, "main:\n");
    fprintf(file, "    push %%rbp\n");
    fprintf(file, "    mov %%rsp, %%rbp\n");
}

void write_assembly_footer(FILE* file) {
    fprintf(file, "    pop %%rbp\n");
    fprintf(file, "    ret\n");
    fprintf(file, "\n");
}

void write_instruction_assembly(FILE* file, IRInstruction* instr) {
    if (!instr) return;
    
    switch (instr->type) {
        case IR_ASSIGN:
            if (instr->arg1) {
                fprintf(file, "    mov $%s, %%rax\n", instr->arg1);
            }
            break;
        case IR_ADD:
            fprintf(file, "    add %%rbx, %%rax\n");
            break;
        case IR_SUB:
            fprintf(file, "    sub %%rbx, %%rax\n");
            break;
        case IR_MUL:
            fprintf(file, "    imul %%rbx, %%rax\n");
            break;
        case IR_DIV:
            fprintf(file, "    idiv %%rbx\n");
            break;
        case IR_RETURN:
            if (instr->arg1) {
                fprintf(file, "    mov $%s, %%rax\n", instr->arg1);
            }
            fprintf(file, "    ret\n");
            break;
        default:
            // Para otras instrucciones, generar código básico
            fprintf(file, "    # Instrucción: %d\n", instr->type);
            break;
    }
}