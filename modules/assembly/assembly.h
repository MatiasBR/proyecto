#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include "../intermediate/intermediate.h"

// Function declarations for assembly code generation
int generate_assembly_code(IRCode* ir_code, const char* output_filename);
void generate_prologue(FILE* file);
void generate_epilogue(FILE* file);
void generate_instruction(FILE* file, IRInstruction* instruction);
void generate_arithmetic(FILE* file, IRInstruction* instruction);
void generate_comparison(FILE* file, IRInstruction* instruction);
void generate_logical(FILE* file, IRInstruction* instruction);
void generate_control_flow(FILE* file, IRInstruction* instruction);
void generate_function_call(FILE* file, IRInstruction* instruction);

// Utility functions
const char* get_register_for_temp(const char* temp);
const char* get_assembly_operator(IRType type);
void allocate_stack_space(FILE* file, int bytes);
void deallocate_stack_space(FILE* file, int bytes);

#endif
