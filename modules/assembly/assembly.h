#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include "../intermediate/intermediate.h"
#include <stdio.h>

// Estructura para manejo de variables y registros
typedef struct {
    char* name;
    int offset;  // Offset en el stack frame
    int is_temp;
} Variable;

typedef struct {
    Variable* variables;
    int var_count;
    int temp_count;
    int label_count;
    int current_offset;
} AssemblyContext;

// Funciones principales
int generate_assembly_code(IRCode* ir_code, const char* output_filename);

// Funciones auxiliares
void write_assembly_header(FILE* file);
void write_assembly_footer(FILE* file);
void write_instruction_assembly(FILE* file, IRInstruction* instr, AssemblyContext* ctx);

#endif