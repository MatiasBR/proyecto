#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "../intermediate/intermediate.h"

typedef enum {
    OPT_CONSTANT_FOLDING,
    OPT_DEAD_CODE_ELIMINATION,
    OPT_COPY_PROPAGATION,
    OPT_COMMON_SUBEXPRESSION_ELIMINATION,
    OPT_ALL
} OptimizationType;

IRCode* optimize_ir_code(IRCode* ir_code, OptimizationType opt_type);
IRCode* optimize_all(IRCode* ir_code);
IRCode* constant_folding(IRCode* ir_code);
IRCode* dead_code_elimination(IRCode* ir_code);
IRCode* copy_propagation(IRCode* ir_code);
IRCode* common_subexpression_elimination(IRCode* ir_code);

int is_constant(char* value);
int get_constant_value(char* value);
char* int_to_string(int value);
IRCode* copy_ir_code(IRCode* ir_code);

#endif

