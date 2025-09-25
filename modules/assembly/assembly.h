#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include "../intermediate/intermediate.h"

// Funciones principales
int generate_assembly_code(IRCode* ir_code, const char* output_filename);

// Funciones auxiliares
void write_assembly_header(FILE* file);
void write_assembly_footer(FILE* file);
void write_instruction_assembly(FILE* file, IRInstruction* instr);

#endif