#define _GNU_SOURCE
#include "optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int is_constant(char* value) {
    if (!value) return 0;
    
    if (value[0] == '-' || isdigit(value[0])) {
        for (int i = (value[0] == '-' ? 1 : 0); value[i] != '\0'; i++) {
            if (!isdigit(value[i])) {
                return 0;
            }
        }
        return 1;
    }
    
    return 0;
}

int get_constant_value(char* value) {
    if (!value) return 0;
    return atoi(value);
}

char* int_to_string(int value) {
    char* str = malloc(16);
    if (str) {
        snprintf(str, 16, "%d", value);
    }
    return str;
}

IRCode* copy_ir_code(IRCode* ir_code) {
    if (!ir_code) return NULL;
    
    IRCode* new_ir = malloc(sizeof(IRCode));
    if (!new_ir) return NULL;
    
    new_ir->instructions = NULL;
    new_ir->temp_count = ir_code->temp_count;
    
    IRInstruction* current = ir_code->instructions;
    IRInstruction* last = NULL;
    
    while (current) {
        IRInstruction* new_instr = create_ir_instruction(
            current->type,
            current->result,
            current->arg1,
            current->arg2,
            current->label
        );
        
        if (!new_ir->instructions) {
            new_ir->instructions = new_instr;
        } else {
            last->next = new_instr;
        }
        last = new_instr;
        current = current->next;
    }
    
    return new_ir;
}

IRCode* constant_folding(IRCode* ir_code) {
    if (!ir_code) return NULL;
    
    IRCode* optimized = copy_ir_code(ir_code);
    if (!optimized) return ir_code;
    
    IRInstruction* current = optimized->instructions;
    int changed = 1;
    
    while (changed) {
        changed = 0;
        current = optimized->instructions;
        
        while (current) {
            int can_fold = 0;
            int result = 0;
            
            if (current->arg1 && current->arg2 && is_constant(current->arg1) && is_constant(current->arg2)) {
                can_fold = 1;
                int val1 = get_constant_value(current->arg1);
                int val2 = get_constant_value(current->arg2);
                
                switch (current->type) {
                    case IR_ADD:
                        result = val1 + val2;
                        break;
                    case IR_SUB:
                        result = val1 - val2;
                        break;
                    case IR_MUL:
                        result = val1 * val2;
                        break;
                    case IR_DIV:
                        if (val2 != 0) {
                            result = val1 / val2;
                        } else {
                            can_fold = 0;
                        }
                        break;
                    case IR_MOD:
                        if (val2 != 0) {
                            result = val1 % val2;
                        } else {
                            can_fold = 0;
                        }
                        break;
                    case IR_LT:
                        result = (val1 < val2) ? 1 : 0;
                        break;
                    case IR_GT:
                        result = (val1 > val2) ? 1 : 0;
                        break;
                    case IR_EQ:
                        result = (val1 == val2) ? 1 : 0;
                        break;
                    case IR_AND:
                        result = (val1 && val2) ? 1 : 0;
                        break;
                    case IR_OR:
                        result = (val1 || val2) ? 1 : 0;
                        break;
                    default:
                        can_fold = 0;
                        break;
                }
            } else if (current->arg1 && is_constant(current->arg1) && !current->arg2) {
                can_fold = 1;
                int val1 = get_constant_value(current->arg1);
                
                switch (current->type) {
                    case IR_NOT:
                        result = val1 ? 0 : 1;
                        break;
                    case IR_NEG:
                        result = -val1;
                        break;
                    default:
                        can_fold = 0;
                        break;
                }
            }
            
            if (can_fold && current->result) {
                char* result_str = int_to_string(result);
                if (result_str) {
                    free(current->arg1);
                    free(current->arg2);
                    current->arg1 = result_str;
                    current->arg2 = NULL;
                    current->type = IR_ASSIGN;
                    changed = 1;
                }
            }
            
            current = current->next;
        }
    }
    
    return optimized;
}

IRCode* dead_code_elimination(IRCode* ir_code) {
    if (!ir_code) return NULL;
    
    IRCode* optimized = copy_ir_code(ir_code);
    if (!optimized) return ir_code;
    
    int* reachable = NULL;
    int label_count = 0;
    int capacity = 100;
    reachable = malloc(capacity * sizeof(int));
    
    IRInstruction* current = optimized->instructions;
    while (current) {
        if (current->type == IR_LABEL && current->label) {
            if (label_count >= capacity) {
                capacity *= 2;
                reachable = realloc(reachable, capacity * sizeof(int));
            }
            reachable[label_count] = 0;
            label_count++;
        }
        current = current->next;
    }
    
    int changed = 1;
    while (changed) {
        changed = 0;
        current = optimized->instructions;
        int label_idx = 0;
        
        while (current) {
            if (current->type == IR_LABEL) {
                if (label_idx == 0 || (label_idx > 0 && reachable[label_idx - 1])) {
                    reachable[label_idx] = 1;
                    changed = 1;
                }
                label_idx++;
            } else if (current->type == IR_GOTO || current->type == IR_IF_GOTO) {
                if (current->label) {
                    int target_idx = 0;
                    IRInstruction* search = optimized->instructions;
                    while (search && search != current) {
                        if (search->type == IR_LABEL && 
                            search->label && 
                            strcmp(search->label, current->label) == 0) {
                            if (target_idx < label_count) {
                                if (!reachable[target_idx]) {
                                    reachable[target_idx] = 1;
                                    changed = 1;
                                }
                            }
                            break;
                        }
                        if (search->type == IR_LABEL) target_idx++;
                        search = search->next;
                    }
                }
            }
            
            if (current->type == IR_FUNC_START || 
                current->type == IR_CALL ||
                current->type == IR_RETURN) {
                IRInstruction* next = current->next;
                while (next && next->type != IR_LABEL && next->type != IR_FUNC_END) {
                    next = next->next;
                }
                if (next && next->type == IR_LABEL) {
                    int target_idx = 0;
                    IRInstruction* search = optimized->instructions;
                    while (search && search != next) {
                        if (search->type == IR_LABEL) target_idx++;
                        search = search->next;
                    }
                    if (target_idx < label_count && !reachable[target_idx]) {
                        reachable[target_idx] = 1;
                        changed = 1;
                    }
                }
            }
            
            current = current->next;
        }
    }
    
    IRInstruction* prev = NULL;
    current = optimized->instructions;
    int label_idx = 0;
    
    while (current) {
        int should_remove = 0;
        
        if (current->type == IR_LABEL) {
            if (label_idx < label_count && !reachable[label_idx]) {
                should_remove = 1;
            }
            label_idx++;
        } else {
            int is_unreachable = 1;
            
            IRInstruction* before = optimized->instructions;
            while (before && before != current) {
                if (before->type == IR_LABEL) {
                    int idx = 0;
                    IRInstruction* count = optimized->instructions;
                    while (count && count != before) {
                        if (count->type == IR_LABEL) idx++;
                        count = count->next;
                    }
                    if (idx < label_count && reachable[idx]) {
                        is_unreachable = 0;
                        break;
                    }
                } else if (before->type == IR_FUNC_START || 
                          before->type == IR_GOTO ||
                          before->type == IR_IF_GOTO ||
                          before->type == IR_CALL ||
                          before->type == IR_RETURN) {
                    is_unreachable = 0;
                    break;
                }
                before = before->next;
            }
            
            if (is_unreachable && optimized->instructions != current) {
                should_remove = 1;
            }
        }
        
        if (should_remove) {
            IRInstruction* next = current->next;
            if (prev) {
                prev->next = next;
            } else {
                optimized->instructions = next;
            }
            
            if (current->result) free(current->result);
            if (current->arg1) free(current->arg1);
            if (current->arg2) free(current->arg2);
            if (current->label) free(current->label);
            free(current);
            current = next;
        } else {
            prev = current;
            current = current->next;
        }
    }
    
    free(reachable);
    return optimized;
}

IRCode* copy_propagation(IRCode* ir_code) {
    if (!ir_code) return NULL;
    
    IRCode* optimized = copy_ir_code(ir_code);
    if (!optimized) return ir_code;
    
    int changed = 1;
    while (changed) {
        changed = 0;
        IRInstruction* current = optimized->instructions;
        
        while (current) {
            if (current->type == IR_ASSIGN && current->result && current->arg1) {
                char* var = current->result;
                char* value = current->arg1;
                
                if (!is_constant(var) && (is_constant(value) || strcmp(var, value) != 0)) {
                    IRInstruction* search = current->next;
                    int var_redefined = 0;
                    
                    while (search && !var_redefined) {
                        if (search->type == IR_FUNC_START || 
                            search->type == IR_FUNC_END ||
                            search->type == IR_LABEL) {
                            break;
                        }
                        
                        if (search->result && strcmp(search->result, var) == 0) {
                            var_redefined = 1;
                        }
                        
                        if (search->arg1 && strcmp(search->arg1, var) == 0) {
                            if (!var_redefined) {
                                free(search->arg1);
                                search->arg1 = strdup(value);
                                changed = 1;
                            }
                        }
                        
                        if (search->arg2 && strcmp(search->arg2, var) == 0) {
                            if (!var_redefined) {
                                free(search->arg2);
                                search->arg2 = strdup(value);
                                changed = 1;
                            }
                        }
                        
                        search = search->next;
                    }
                }
            }
            
            current = current->next;
        }
    }
    
    IRInstruction* prev = NULL;
    IRInstruction* current = optimized->instructions;
    
    while (current) {
        if (current->type == IR_ASSIGN && current->result && current->arg1) {
            if (strcmp(current->result, current->arg1) == 0) {
                IRInstruction* next = current->next;
                if (prev) {
                    prev->next = next;
                } else {
                    optimized->instructions = next;
                }
                
                if (current->result) free(current->result);
                if (current->arg1) free(current->arg1);
                free(current);
                current = next;
                continue;
            }
        }
        prev = current;
        current = current->next;
    }
    
    return optimized;
}

typedef struct ExprCache {
    char* expr_key;
    char* result_var;
    struct ExprCache* next;
} ExprCache;

IRCode* common_subexpression_elimination(IRCode* ir_code) {
    if (!ir_code) return NULL;
    
    IRCode* optimized = copy_ir_code(ir_code);
    if (!optimized) return ir_code;
    
    ExprCache* cache = NULL;
    IRInstruction* current = optimized->instructions;
    
    while (current) {
        if (current->arg1 && current->arg2 && 
            (current->type == IR_ADD || current->type == IR_SUB || 
             current->type == IR_MUL || current->type == IR_DIV ||
             current->type == IR_MOD || current->type == IR_LT ||
             current->type == IR_GT || current->type == IR_EQ ||
             current->type == IR_AND || current->type == IR_OR)) {
            
            char expr_key[256];
            snprintf(expr_key, sizeof(expr_key), "%d:%s:%s", 
                    current->type, current->arg1, current->arg2);
            
            ExprCache* search = cache;
            ExprCache* found = NULL;
            
            while (search) {
                if (strcmp(search->expr_key, expr_key) == 0) {
                    found = search;
                    break;
                }
                search = search->next;
            }
            
            if (found && current->result) {
                IRInstruction* replace = create_ir_instruction(
                    IR_ASSIGN, current->result, found->result_var, NULL, NULL
                );
                
                if (replace) {
                    replace->next = current->next;
                    if (current == optimized->instructions) {
                        optimized->instructions = replace;
                    } else {
                        IRInstruction* prev = optimized->instructions;
                        while (prev && prev->next != current) {
                            prev = prev->next;
                        }
                        if (prev) prev->next = replace;
                    }
                    
                    if (current->result) free(current->result);
                    if (current->arg1) free(current->arg1);
                    if (current->arg2) free(current->arg2);
                    free(current);
                    current = replace;
                }
            } else if (current->result) {
                ExprCache* new_entry = malloc(sizeof(ExprCache));
                if (new_entry) {
                    new_entry->expr_key = strdup(expr_key);
                    new_entry->result_var = strdup(current->result);
                    new_entry->next = cache;
                    cache = new_entry;
                }
            }
        }
        
        if (current->type == IR_FUNC_START || current->type == IR_FUNC_END ||
            current->type == IR_LABEL) {
            ExprCache* temp = cache;
            while (temp) {
                ExprCache* next = temp->next;
                free(temp->expr_key);
                free(temp->result_var);
                free(temp);
                temp = next;
            }
            cache = NULL;
        }
        
        current = current->next;
    }
    
    ExprCache* temp = cache;
    while (temp) {
        ExprCache* next = temp->next;
        free(temp->expr_key);
        free(temp->result_var);
        free(temp);
        temp = next;
    }
    
    return optimized;
}

IRCode* optimize_all(IRCode* ir_code) {
    if (!ir_code) return NULL;
    
    IRCode* result = ir_code;
    
    result = constant_folding(result);
    if (result != ir_code) free_ir_code(ir_code);
    ir_code = result;
    
    result = copy_propagation(result);
    if (result != ir_code) free_ir_code(ir_code);
    ir_code = result;
    
    result = common_subexpression_elimination(result);
    if (result != ir_code) free_ir_code(ir_code);
    ir_code = result;
    
    result = dead_code_elimination(result);
    if (result != ir_code) free_ir_code(ir_code);
    
    return result;
}

IRCode* optimize_ir_code(IRCode* ir_code, OptimizationType opt_type) {
    if (!ir_code) return NULL;
    
    switch (opt_type) {
        case OPT_CONSTANT_FOLDING:
            return constant_folding(ir_code);
        case OPT_DEAD_CODE_ELIMINATION:
            return dead_code_elimination(ir_code);
        case OPT_COPY_PROPAGATION:
            return copy_propagation(ir_code);
        case OPT_COMMON_SUBEXPRESSION_ELIMINATION:
            return common_subexpression_elimination(ir_code);
        case OPT_ALL:
            return optimize_all(ir_code);
        default:
            return ir_code;
    }
}

