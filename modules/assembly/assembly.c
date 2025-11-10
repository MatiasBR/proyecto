#define _GNU_SOURCE
#include "assembly.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static AssemblyContext* create_assembly_context();
static void free_assembly_context(AssemblyContext* ctx);
static void add_variable(AssemblyContext* ctx, const char* name, int is_temp);
static int get_variable_offset(AssemblyContext* ctx, const char* name);
static char* generate_assembly_label(AssemblyContext* ctx);
static char* get_register_for_temp(int temp_num);

int generate_assembly_code(IRCode* ir_code, const char* output_filename) {
    if (!ir_code || !output_filename) {
        return 0;
    }
    
    FILE* file = fopen(output_filename, "w");
    if (!file) {
        return 0;
    }
    
    AssemblyContext* ctx = create_assembly_context();
    if (!ctx) {
        fclose(file);
        return 0;
    }
    
    write_assembly_header(file);
    
    IRInstruction* current = ir_code->instructions;
    while (current) {
        write_instruction_assembly(file, current, ctx);
        current = current->next;
    }
    
    write_assembly_footer(file);
    
    fclose(file);
    free_assembly_context(ctx);
    
    return 1;
}

void write_assembly_header(FILE* file) {
    fprintf(file, ".text\n");
    fprintf(file, ".globl _start\n");
    fprintf(file, ".globl _main\n");
    fprintf(file, "\n");
    fprintf(file, "_start:\n");
    fprintf(file, "    call _main\n");
    fprintf(file, "#ifdef __APPLE__\n");
    fprintf(file, "    mov $0x2000001, %%rax\n");
    fprintf(file, "#else\n");
    fprintf(file, "    mov $60, %%rax\n");
    fprintf(file, "#endif\n");
    fprintf(file, "    mov $0, %%rdi\n");
    fprintf(file, "    syscall\n");
    fprintf(file, "\n");
    fprintf(file, "_main:\n");
    fprintf(file, "    push %%rbp\n");
    fprintf(file, "    mov %%rsp, %%rbp\n");
    fprintf(file, "    sub $32, %%rsp\n");
}

void write_assembly_footer(FILE* file) {
    fprintf(file, "    mov %%rbp, %%rsp\n");
    fprintf(file, "    pop %%rbp\n");
    fprintf(file, "    ret\n");
    fprintf(file, "\n");
}

void write_instruction_assembly(FILE* file, IRInstruction* instr, AssemblyContext* ctx) {
    if (!instr || !file) return;
    
    switch (instr->type) {
        case IR_ASSIGN:
            if (instr->arg1 && instr->result) {
                fprintf(file, "    mov $%s, %%rax\n", instr->arg1);
                fprintf(file, "    mov %%rax, -%d(%%rbp)\n", get_variable_offset(ctx, instr->result));
            }
            break;
            
        case IR_ADD:
            fprintf(file, "    mov -%d(%%rbp), %%rax\n", get_variable_offset(ctx, instr->arg1));
            fprintf(file, "    mov -%d(%%rbp), %%rbx\n", get_variable_offset(ctx, instr->arg2));
            fprintf(file, "    add %%rbx, %%rax\n");
            fprintf(file, "    mov %%rax, -%d(%%rbp)\n", get_variable_offset(ctx, instr->result));
            break;
            
        case IR_SUB:
            fprintf(file, "    mov -%d(%%rbp), %%rax\n", get_variable_offset(ctx, instr->arg1));
            fprintf(file, "    mov -%d(%%rbp), %%rbx\n", get_variable_offset(ctx, instr->arg2));
            fprintf(file, "    sub %%rbx, %%rax\n");
            fprintf(file, "    mov %%rax, -%d(%%rbp)\n", get_variable_offset(ctx, instr->result));
            break;
            
        case IR_MUL:
            fprintf(file, "    mov -%d(%%rbp), %%rax\n", get_variable_offset(ctx, instr->arg1));
            fprintf(file, "    mov -%d(%%rbp), %%rbx\n", get_variable_offset(ctx, instr->arg2));
            fprintf(file, "    imul %%rbx, %%rax\n");
            fprintf(file, "    mov %%rax, -%d(%%rbp)\n", get_variable_offset(ctx, instr->result));
            break;
            
        case IR_DIV:
            fprintf(file, "    mov -%d(%%rbp), %%rax\n", get_variable_offset(ctx, instr->arg1));
            fprintf(file, "    mov -%d(%%rbp), %%rbx\n", get_variable_offset(ctx, instr->arg2));
            fprintf(file, "    cqo\n");
            fprintf(file, "    idiv %%rbx\n");
            fprintf(file, "    mov %%rax, -%d(%%rbp)\n", get_variable_offset(ctx, instr->result));
            break;
            
        case IR_LT:
            fprintf(file, "    mov -%d(%%rbp), %%rax\n", get_variable_offset(ctx, instr->arg1));
            fprintf(file, "    mov -%d(%%rbp), %%rbx\n", get_variable_offset(ctx, instr->arg2));
            fprintf(file, "    cmp %%rbx, %%rax\n");
            fprintf(file, "    setl %%al\n");
            fprintf(file, "    movzx %%al, %%rax\n");
            fprintf(file, "    mov %%rax, -%d(%%rbp)\n", get_variable_offset(ctx, instr->result));
            break;
            
        case IR_GT:
            fprintf(file, "    mov -%d(%%rbp), %%rax\n", get_variable_offset(ctx, instr->arg1));
            fprintf(file, "    mov -%d(%%rbp), %%rbx\n", get_variable_offset(ctx, instr->arg2));
            fprintf(file, "    cmp %%rbx, %%rax\n");
            fprintf(file, "    setg %%al\n");
            fprintf(file, "    movzx %%al, %%rax\n");
            fprintf(file, "    mov %%rax, -%d(%%rbp)\n", get_variable_offset(ctx, instr->result));
            break;
            
        case IR_EQ:
            fprintf(file, "    mov -%d(%%rbp), %%rax\n", get_variable_offset(ctx, instr->arg1));
            fprintf(file, "    mov -%d(%%rbp), %%rbx\n", get_variable_offset(ctx, instr->arg2));
            fprintf(file, "    cmp %%rbx, %%rax\n");
            fprintf(file, "    sete %%al\n");
            fprintf(file, "    movzx %%al, %%rax\n");
            fprintf(file, "    mov %%rax, -%d(%%rbp)\n", get_variable_offset(ctx, instr->result));
            break;
            
        case IR_AND:
            fprintf(file, "    mov -%d(%%rbp), %%rax\n", get_variable_offset(ctx, instr->arg1));
            fprintf(file, "    mov -%d(%%rbp), %%rbx\n", get_variable_offset(ctx, instr->arg2));
            fprintf(file, "    and %%rbx, %%rax\n");
            fprintf(file, "    mov %%rax, -%d(%%rbp)\n", get_variable_offset(ctx, instr->result));
            break;
            
        case IR_OR:
            fprintf(file, "    mov -%d(%%rbp), %%rax\n", get_variable_offset(ctx, instr->arg1));
            fprintf(file, "    mov -%d(%%rbp), %%rbx\n", get_variable_offset(ctx, instr->arg2));
            fprintf(file, "    or %%rbx, %%rax\n");
            fprintf(file, "    mov %%rax, -%d(%%rbp)\n", get_variable_offset(ctx, instr->result));
            break;
            
        case IR_NOT:
            fprintf(file, "    mov -%d(%%rbp), %%rax\n", get_variable_offset(ctx, instr->arg1));
            fprintf(file, "    not %%rax\n");
            fprintf(file, "    mov %%rax, -%d(%%rbp)\n", get_variable_offset(ctx, instr->result));
            break;
            
        case IR_NEG:
            fprintf(file, "    mov -%d(%%rbp), %%rax\n", get_variable_offset(ctx, instr->arg1));
            fprintf(file, "    neg %%rax\n");
            fprintf(file, "    mov %%rax, -%d(%%rbp)\n", get_variable_offset(ctx, instr->result));
            break;
            
        case IR_LABEL:
            if (instr->label) {
                fprintf(file, "%s:\n", instr->label);
            }
            break;
            
        case IR_GOTO:
            if (instr->label) {
                fprintf(file, "    jmp %s\n", instr->label);
            }
            break;
            
        case IR_IF_GOTO:
            if (instr->arg1 && instr->label) {
                fprintf(file, "    mov -%d(%%rbp), %%rax\n", get_variable_offset(ctx, instr->arg1));
                fprintf(file, "    cmp $0, %%rax\n");
                fprintf(file, "    jne %s\n", instr->label);
            }
            break;
            
        case IR_RETURN:
            if (instr->arg1) {
                fprintf(file, "    mov -%d(%%rbp), %%rax\n", get_variable_offset(ctx, instr->arg1));
            } else {
                fprintf(file, "    mov $0, %%rax\n");
            }
            fprintf(file, "    jmp .L_end_main\n");
            break;
            
        case IR_CALL:
            if (instr->arg1) {
                fprintf(file, "    call %s\n", instr->arg1);
                if (instr->result) {
                    fprintf(file, "    mov %%rax, -%d(%%rbp)\n", get_variable_offset(ctx, instr->result));
                }
            }
            break;
            
        case IR_PARAM:
            if (instr->arg1) {
                fprintf(file, "    mov -%d(%%rbp), %%rdi\n", get_variable_offset(ctx, instr->arg1));
            }
            break;
            
        case IR_FUNC_START:
            if (instr->arg1) {
                fprintf(file, "%s:\n", instr->arg1);
                fprintf(file, "    push %%rbp\n");
                fprintf(file, "    mov %%rsp, %%rbp\n");
            }
            break;
            
        case IR_FUNC_END:
            if (instr->arg1) {
                fprintf(file, "    mov %%rbp, %%rsp\n");
                fprintf(file, "    pop %%rbp\n");
                fprintf(file, "    ret\n");
            }
            break;
            
        default:
            break;
    }
}

static AssemblyContext* create_assembly_context() {
    AssemblyContext* ctx = malloc(sizeof(AssemblyContext));
    if (!ctx) return NULL;
    
    ctx->variables = NULL;
    ctx->var_count = 0;
    ctx->temp_count = 0;
    ctx->label_count = 0;
    ctx->current_offset = 8;
    
    return ctx;
}

static void free_assembly_context(AssemblyContext* ctx) {
    if (!ctx) return;
    
    for (int i = 0; i < ctx->var_count; i++) {
        free(ctx->variables[i].name);
    }
    free(ctx->variables);
    free(ctx);
}

static void add_variable(AssemblyContext* ctx, const char* name, int is_temp) {
    if (!ctx || !name) return;
    
    ctx->variables = realloc(ctx->variables, (ctx->var_count + 1) * sizeof(Variable));
    if (!ctx->variables) return;
    
    ctx->variables[ctx->var_count].name = strdup(name);
    ctx->variables[ctx->var_count].offset = ctx->current_offset;
    ctx->variables[ctx->var_count].is_temp = is_temp;
    
    ctx->var_count++;
    ctx->current_offset += 8;
}

static int get_variable_offset(AssemblyContext* ctx, const char* name) {
    if (!ctx || !name) return 8;
    
    for (int i = 0; i < ctx->var_count; i++) {
        if (strcmp(ctx->variables[i].name, name) == 0) {
            return ctx->variables[i].offset;
        }
    }
    
    add_variable(ctx, name, 0);
    return ctx->variables[ctx->var_count - 1].offset;
}

static char* generate_assembly_label(AssemblyContext* ctx) {
    if (!ctx) return NULL;
    
    char* label = malloc(32);
    if (!label) return NULL;
    
    snprintf(label, 32, ".L%d", ctx->label_count++);
    return label;
}

static char* get_register_for_temp(int temp_num) {
    switch (temp_num % 4) {
        case 0: return "rax";
        case 1: return "rbx";
        case 2: return "rcx";
        case 3: return "rdx";
        default: return "rax";
    }
}