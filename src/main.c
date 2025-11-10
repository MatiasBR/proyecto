#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../modules/ast/ast.h"
#include "../modules/semantic/semantic.h"
#include "../modules/intermediate/intermediate.h"
#include "../modules/assembly/assembly.h"
#include "../modules/optimizer/optimizer.h"
#include "../modules/parser/parser.h"

extern FILE* yyin;
extern int yyparse(void);
extern int yylex(void);
extern YYSTYPE yylval;
extern ASTNode* root;
extern int yylineno;

const char* token_to_string(int token) {
    switch (token) {
        case PROGRAM: return "PROGRAM";
        case BOOL: return "BOOL";
        case ELSE: return "ELSE";
        case EXTERN: return "EXTERN";
        case FALSE: return "FALSE";
        case IF: return "IF";
        case INTEGER: return "INTEGER";
        case RETURN: return "RETURN";
        case THEN: return "THEN";
        case TRUE: return "TRUE";
        case VOID: return "VOID";
        case WHILE: return "WHILE";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case MULTIPLY: return "MULTIPLY";
        case DIVIDE: return "DIVIDE";
        case MODULO: return "MODULO";
        case LESS: return "LESS";
        case GREATER: return "GREATER";
        case EQUAL: return "EQUAL";
        case AND: return "AND";
        case OR: return "OR";
        case NOT: return "NOT";
        case ASSIGN: return "ASSIGN";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case SEMICOLON: return "SEMICOLON";
        case COMMA: return "COMMA";
        case INTEGER_LITERAL: return "INTEGER_LITERAL";
        case IDENTIFIER: return "IDENTIFIER";
        case 0: return "EOF";
        default: return "UNKNOWN";
    }
}

void print_usage(const char* program_name) {
    printf("Uso: %s [opcion] nombreArchivo.ctds\n", program_name);
    printf("Opciones:\n");
    printf("  -o <salida>        Renombra el archivo ejecutable a <salida>\n");
    printf("  -target <etapa>    Compila hasta la etapa especificada (scan, parse, codinter, assembly)\n");
    printf("  -opt [optimizacion] Realiza optimizaciones. all realiza todas las optimizaciones soportadas.\n");
    printf("  -debug             Imprime información de debugging\n");
    printf("  -h, --help         Muestra esta ayuda\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    char* input_file = NULL;
    char* output_file = NULL;
    char* target_stage = NULL;
    int debug_mode = 0;
    int optimize = 0;
    char* opt_type_str = "all";
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                output_file = argv[++i];
            } else {
                fprintf(stderr, "Error: -o requiere un nombre de archivo\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-target") == 0) {
            if (i + 1 < argc) {
                target_stage = argv[++i];
            } else {
                fprintf(stderr, "Error: -target requiere una etapa\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-debug") == 0) {
            debug_mode = 1;
        } else if (strcmp(argv[i], "-opt") == 0) {
            optimize = 1;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                opt_type_str = argv[++i];
            }
        } else if (argv[i][0] != '-') {
            input_file = argv[i];
        } else {
            fprintf(stderr, "Error: Opción desconocida '%s'\n", argv[i]);
            return 1;
        }
    }
    
    if (!input_file) {
        fprintf(stderr, "Error: No se especificó archivo de entrada\n");
        return 1;
    }
    
    if (input_file[0] == '-') {
        fprintf(stderr, "Error: El nombre del archivo no puede empezar con '-'\n");
        return 1;
    }
    
    char* ext = strrchr(input_file, '.');
    if (!ext || strcmp(ext, ".ctds") != 0) {
        fprintf(stderr, "Error: El archivo debe tener extensión .ctds\n");
        return 1;
    }
    
    yyin = fopen(input_file, "r");
    if (!yyin) {
        fprintf(stderr, "Error: No se pudo abrir el archivo '%s'\n", input_file);
        return 1;
    }
    
    char base_filename[256];
    strcpy(base_filename, input_file);
    char* dot = strrchr(base_filename, '.');
    if (dot) *dot = '\0';
    
    if (!target_stage) {
        target_stage = "optimizer";
    }
    
    if (debug_mode) {
        printf("Compilando: %s\n", input_file);
        printf("Etapa objetivo: %s\n", target_stage);
    }
    
    if (strcmp(target_stage, "scan") == 0) {
        if (debug_mode) {
            printf("=== Análisis Léxico ===\n");
        }
        
        char output_filename[256];
        strcpy(output_filename, base_filename);
        strcat(output_filename, ".lex");
        
        FILE* lex_file = fopen(output_filename, "w");
        if (!lex_file) {
            fprintf(stderr, "Error: No se pudo crear el archivo '%s'\n", output_filename);
            fclose(yyin);
            return 1;
        }
        
        int token;
        int error_occurred = 0;
        while ((token = yylex()) != 0) {
            if (token == ERROR) {
                error_occurred = 1;
                continue;
            }
            fprintf(lex_file, "%s", token_to_string(token));
            if (token == INTEGER_LITERAL) {
                fprintf(lex_file, " %d", yylval.int_val);
            } else if (token == IDENTIFIER) {
                fprintf(lex_file, " %s", yylval.string_val);
            }
            fprintf(lex_file, "\n");
        }
        
        fclose(lex_file);
        fclose(yyin);
        
        if (error_occurred) {
            return 1;
        }
        
        if (debug_mode) {
            printf("Análisis léxico exitoso. Tokens guardados en: %s\n", output_filename);
        }
        
        return 0;
    }
    
    if (strcmp(target_stage, "parse") == 0 || strcmp(target_stage, "codinter") == 0 || 
        strcmp(target_stage, "assembly") == 0 || strcmp(target_stage, "optimizer") == 0) {
        
        if (debug_mode) {
            printf("=== Análisis Léxico y Sintáctico ===\n");
        }
        
        int parse_result = yyparse();
        if (parse_result != 0) {
            fprintf(stderr, "Error en el análisis sintáctico\n");
            fclose(yyin);
            return 1;
        }
        
        if (debug_mode) {
            printf("Análisis sintáctico exitoso\n");
        }
        
        if (strcmp(target_stage, "parse") == 0) {
            char output_filename[256];
            strcpy(output_filename, base_filename);
            strcat(output_filename, ".sint");
            
            FILE* sint_file = fopen(output_filename, "w");
            if (sint_file) {
                fprintf(sint_file, "Análisis sintáctico exitoso\n");
                fclose(sint_file);
            }
            fclose(yyin);
            return 0;
        }
    }
    
    if (strcmp(target_stage, "codinter") == 0 || strcmp(target_stage, "assembly") == 0 || 
        strcmp(target_stage, "optimizer") == 0) {
        
        if (debug_mode) {
            printf("\n=== Análisis Semántico ===\n");
        }
        
        int semantic_result = semantic_analysis(root);
        if (semantic_result == 0) {
            fprintf(stderr, "Error en el análisis semántico\n");
            fclose(yyin);
            return 1;
        }
        
        if (debug_mode) {
            printf("Análisis semántico exitoso\n");
        }
        
        char output_filename[256];
        strcpy(output_filename, base_filename);
        strcat(output_filename, ".sem");
        
        FILE* sem_file = fopen(output_filename, "w");
        if (sem_file) {
            fprintf(sem_file, "Análisis semántico exitoso\n");
            fclose(sem_file);
        }
    }
    
    if (strcmp(target_stage, "codinter") == 0 || strcmp(target_stage, "assembly") == 0 || 
        strcmp(target_stage, "optimizer") == 0) {
        
        if (debug_mode) {
            printf("\n=== Generación de Código Intermedio ===\n");
        }
        
        IRCode* ir_code = generate_intermediate_code(root);
        if (!ir_code) {
            fprintf(stderr, "Error: No se pudo generar código intermedio\n");
            fclose(yyin);
            return 1;
        }
        
        OptimizationType opt_type = OPT_ALL;
        if (optimize && strcmp(opt_type_str, "all") != 0) {
            if (strcmp(opt_type_str, "constant_folding") == 0) {
                opt_type = OPT_CONSTANT_FOLDING;
            } else if (strcmp(opt_type_str, "dead_code") == 0) {
                opt_type = OPT_DEAD_CODE_ELIMINATION;
            } else if (strcmp(opt_type_str, "copy_propagation") == 0) {
                opt_type = OPT_COPY_PROPAGATION;
            } else if (strcmp(opt_type_str, "cse") == 0) {
                opt_type = OPT_COMMON_SUBEXPRESSION_ELIMINATION;
            }
        }
        
        if (optimize) {
            if (debug_mode) {
                printf("\n=== Aplicando Optimizaciones ===\n");
            }
            IRCode* optimized = optimize_ir_code(ir_code, opt_type);
            if (optimized && optimized != ir_code) {
                free_ir_code(ir_code);
                ir_code = optimized;
                if (debug_mode) {
                    printf("Código optimizado:\n");
                    print_ir_code(ir_code);
                }
            }
        }
        
        if (debug_mode) {
            printf("Código intermedio generado:\n");
            print_ir_code(ir_code);
        }
        
        if (strcmp(target_stage, "codinter") == 0 || strcmp(target_stage, "optimizer") == 0) {
            char output_filename[256];
            strcpy(output_filename, base_filename);
            strcat(output_filename, optimize ? ".opt" : ".ci");
            
            FILE* ci_file = fopen(output_filename, "w");
            if (ci_file) {
            IRInstruction* current = ir_code->instructions;
            while (current) {
                switch (current->type) {
                    case IR_ASSIGN:
                        fprintf(ci_file, "%s = %s\n", current->result, current->arg1);
                        break;
                    case IR_ADD:
                        fprintf(ci_file, "%s = %s + %s\n", current->result, current->arg1, current->arg2);
                        break;
                    case IR_SUB:
                        fprintf(ci_file, "%s = %s - %s\n", current->result, current->arg1, current->arg2);
                        break;
                    case IR_MUL:
                        fprintf(ci_file, "%s = %s * %s\n", current->result, current->arg1, current->arg2);
                        break;
                    case IR_DIV:
                        fprintf(ci_file, "%s = %s / %s\n", current->result, current->arg1, current->arg2);
                        break;
                    case IR_MOD:
                        fprintf(ci_file, "%s = %s %% %s\n", current->result, current->arg1, current->arg2);
                        break;
                    case IR_LT:
                        fprintf(ci_file, "%s = %s < %s\n", current->result, current->arg1, current->arg2);
                        break;
                    case IR_GT:
                        fprintf(ci_file, "%s = %s > %s\n", current->result, current->arg1, current->arg2);
                        break;
                    case IR_EQ:
                        fprintf(ci_file, "%s = %s == %s\n", current->result, current->arg1, current->arg2);
                        break;
                    case IR_AND:
                        fprintf(ci_file, "%s = %s && %s\n", current->result, current->arg1, current->arg2);
                        break;
                    case IR_OR:
                        fprintf(ci_file, "%s = %s || %s\n", current->result, current->arg1, current->arg2);
                        break;
                    case IR_NOT:
                        fprintf(ci_file, "%s = !%s\n", current->result, current->arg1);
                        break;
                    case IR_NEG:
                        fprintf(ci_file, "%s = -%s\n", current->result, current->arg1);
                        break;
                    case IR_LABEL:
                        fprintf(ci_file, "%s:\n", current->label);
                        break;
                    case IR_GOTO:
                        fprintf(ci_file, "goto %s\n", current->label);
                        break;
                    case IR_IF_GOTO:
                        fprintf(ci_file, "if %s goto %s\n", current->arg1, current->label);
                        break;
                    case IR_CALL:
                        fprintf(ci_file, "%s = call %s\n", current->result, current->arg1);
                        break;
                    case IR_RETURN:
                        if (current->arg1) {
                            fprintf(ci_file, "return %s\n", current->arg1);
                        } else {
                            fprintf(ci_file, "return\n");
                        }
                        break;
                    case IR_PARAM:
                        fprintf(ci_file, "param %s\n", current->arg1);
                        break;
                    case IR_FUNC_START:
                        fprintf(ci_file, "function %s start\n", current->arg1);
                        break;
                    case IR_FUNC_END:
                        fprintf(ci_file, "function %s end\n", current->arg1);
                        break;
                }
                current = current->next;
            }
            fclose(ci_file);
            }
            
            free_ir_code(ir_code);
            fclose(yyin);
            return 0;
        }
        
        if (strcmp(target_stage, "assembly") == 0) {
            char ass_filename[256];
            strcpy(ass_filename, base_filename);
            strcat(ass_filename, ".ass");
            
            int assembly_result = generate_assembly_code(ir_code, ass_filename);
            if (!assembly_result) {
                fprintf(stderr, "Error: No se pudo generar código assembly\n");
                free_ir_code(ir_code);
                fclose(yyin);
                return 1;
            }
            
            if (debug_mode) {
                printf("Código assembly generado exitosamente\n");
            }
            
            char exe_filename[256];
            strcpy(exe_filename, base_filename);
            strcat(exe_filename, ".out");
            
            if (output_file) {
                strcpy(exe_filename, output_file);
            }
            
            char compile_cmd[512];
            snprintf(compile_cmd, sizeof(compile_cmd), 
                    "as -o %s.o %s && ld -o %s %s.o", 
                    ass_filename, ass_filename, exe_filename, ass_filename);
            
            if (debug_mode) {
                printf("Compilando: %s\n", compile_cmd);
            }
            
            int compile_result = system(compile_cmd);
            if (compile_result != 0) {
                fprintf(stderr, "Error: No se pudo compilar el código assembly\n");
                free_ir_code(ir_code);
                fclose(yyin);
                return 1;
            }
            
            if (debug_mode) {
                printf("Assembly: %s\n", ass_filename);
                printf("Ejecutable: %s\n", exe_filename);
            }
            
            free_ir_code(ir_code);
        }
    }
    
    fclose(yyin);
    
    return 0;
}
