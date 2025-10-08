#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H

#include "../ast/ast.h"

// Tipos de instrucciones de código intermedio
typedef enum {
    IR_ASSIGN,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_MOD,
    IR_LT,
    IR_GT,
    IR_EQ,
    IR_AND,
    IR_OR,
    IR_NOT,
    IR_NEG,
    IR_LABEL,
    IR_GOTO,
    IR_IF_GOTO,
    IR_CALL,
    IR_RETURN,
    IR_PARAM,
    IR_FUNC_START,
    IR_FUNC_END
} IRInstructionType;

// Estructura para una instrucción IR
typedef struct IRInstruction {
    IRInstructionType type;
    char* result;
    char* arg1;
    char* arg2;
    char* label;
    struct IRInstruction* next;
} IRInstruction;

// Estructura para el código intermedio
typedef struct {
    IRInstruction* instructions;
    int temp_count;
} IRCode;

// Funciones principales
IRCode* generate_intermediate_code(ASTNode* root);
void print_ir_code(IRCode* ir_code);
void free_ir_code(IRCode* ir_code);

// Funciones auxiliares
char* generate_temp();
IRInstruction* create_ir_instruction(IRInstructionType type, char* result, char* arg1, char* arg2, char* label);
void add_instruction(IRCode* ir_code, IRInstruction* instr);
void generate_program_ir(ASTNode* node, IRCode* ir_code);


#endif