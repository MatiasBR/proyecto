#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H

#include "../ast/ast.h"
#include "../common/symbol_table.h"

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
} IRType;

typedef struct IRInstruction {
    IRType type;
    char* result;
    char* arg1;
    char* arg2;
    char* label;
    struct IRInstruction* next;
} IRInstruction;

typedef struct IRCode {
    IRInstruction* instructions;
    int instruction_count;
} IRCode;

// Function declarations
IRCode* generate_intermediate_code(ASTNode* root);
IRInstruction* create_ir_instruction(IRType type, char* result, char* arg1, char* arg2, char* label);
void add_ir_instruction(IRCode* code, IRInstruction* instruction);
void print_ir_code(IRCode* code);
void free_ir_code(IRCode* code);
void free_ir_instruction(IRInstruction* instruction);

// Code generation functions
IRCode* generate_program_code(ASTNode* node);
IRCode* generate_method_code(ASTNode* node);
IRCode* generate_block_code(ASTNode* node);
IRCode* generate_statement_code(ASTNode* node);
IRCode* generate_expr_code(ASTNode* node);

#endif
