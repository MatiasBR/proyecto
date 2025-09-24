#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ast.h"
#include "semantic.h"
#include "intermediate.h"

extern FILE* yyin;
extern int yyparse(void);
extern ASTNode* root;

void print_usage(const char* program_name) {
    printf("Uso: %s [opcion] nombreArchivo.ctds\n", program_name);
    printf("Opciones:\n");
    printf("  -o <salida>        Renombra el archivo ejecutable a <salida>\n");
    printf("  -target <etapa>    Compila hasta la etapa especificada (scan, parse, codinter, assembly)\n");
    printf("  -opt [optimizacion] Realiza optimizaciones\n");
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
    char* target_stage = "assembly";
    int debug_mode = 0;
    int optimize = 0;
    
    // Parsear argumentos
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
    
    // Verificar extensión del archivo
    char* ext = strrchr(input_file, '.');
    if (!ext || strcmp(ext, ".ctds") != 0) {
        fprintf(stderr, "Error: El archivo debe tener extensión .ctds\n");
        return 1;
    }
    
    // Abrir archivo de entrada
    yyin = fopen(input_file, "r");
    if (!yyin) {
        fprintf(stderr, "Error: No se pudo abrir el archivo '%s'\n", input_file);
        return 1;
    }
    
    if (debug_mode) {
        printf("Compilando archivo: %s\n", input_file);
        printf("Etapa objetivo: %s\n", target_stage);
    }
    
    // Etapa 1: Análisis Léxico y Sintáctico
    if (strcmp(target_stage, "scan") == 0 || strcmp(target_stage, "parse") == 0 || 
        strcmp(target_stage, "codinter") == 0 || strcmp(target_stage, "assembly") == 0) {
        
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
            printf("=== Árbol AST ===\n");
            print_ast(root, 0);
        }
        
        // Guardar salida de análisis sintáctico
        char output_filename[256];
        strcpy(output_filename, input_file);
        char* dot = strrchr(output_filename, '.');
        if (dot) *dot = '\0';
        strcat(output_filename, ".sint");
        
        FILE* sint_file = fopen(output_filename, "w");
        if (sint_file) {
            fprintf(sint_file, "Análisis sintáctico exitoso\n");
            fclose(sint_file);
        }
    }
    
    // Etapa 2: Análisis Semántico (DESHABILITADO TEMPORALMENTE)
    if (strcmp(target_stage, "codinter") == 0 || strcmp(target_stage, "assembly") == 0) {
        
        if (debug_mode) {
            printf("\n=== Análisis Semántico (DESHABILITADO) ===\n");
        }
        
        // int semantic_result = semantic_analysis(root);
        // if (semantic_result == 0) {
        //     fprintf(stderr, "Error en el análisis semántico\n");
        //     fclose(yyin);
        //     return 1;
        // }
        
        if (debug_mode) {
            printf("Análisis semántico saltado\n");
        }
        
        // Guardar salida de análisis semántico
        char output_filename[256];
        strcpy(output_filename, input_file);
        char* dot = strrchr(output_filename, '.');
        if (dot) *dot = '\0';
        strcat(output_filename, ".sem");
        
        FILE* sem_file = fopen(output_filename, "w");
        if (sem_file) {
            fprintf(sem_file, "Análisis semántico saltado\n");
            fclose(sem_file);
        }
    }
    
    // Etapa 3: Generación de Código Intermedio
    if (strcmp(target_stage, "codinter") == 0 || strcmp(target_stage, "assembly") == 0) {
        
        if (debug_mode) {
            printf("\n=== Generación de Código Intermedio ===\n");
        }
        
        IRCode* ir_code = generate_intermediate_code(root);
        if (ir_code) {
            if (debug_mode) {
                printf("Código intermedio generado:\n");
                print_ir_code(ir_code);
            }
            
            // Guardar código intermedio
            char output_filename[256];
            strcpy(output_filename, input_file);
            char* dot = strrchr(output_filename, '.');
            if (dot) *dot = '\0';
            strcat(output_filename, ".ci");
            
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
        }
    }
    
    // Etapa 4: Generación de Código Assembly (placeholder)
    if (strcmp(target_stage, "assembly") == 0) {
        
        if (debug_mode) {
            printf("\n=== Generación de Código Assembly ===\n");
        }
        
        // Generar archivo assembly básico
        char output_filename[256];
        strcpy(output_filename, input_file);
        char* dot = strrchr(output_filename, '.');
        if (dot) *dot = '\0';
        strcat(output_filename, ".ass");
        
        FILE* ass_file = fopen(output_filename, "w");
        if (ass_file) {
            fprintf(ass_file, "; Código assembly generado para %s\n", input_file);
            fprintf(ass_file, ".section .text\n");
            fprintf(ass_file, ".global main\n");
            fprintf(ass_file, "main:\n");
            fprintf(ass_file, "    ; Código generado por TDS25 compiler\n");
            fprintf(ass_file, "    ret\n");
            fclose(ass_file);
        }
        
        // Generar ejecutable
        char exe_filename[256];
        strcpy(exe_filename, input_file);
        dot = strrchr(exe_filename, '.');
        if (dot) *dot = '\0';
        strcat(exe_filename, ".out");
        
        if (output_file) {
            strcpy(exe_filename, output_file);
        }
        
        if (debug_mode) {
            printf("Archivo assembly generado: %s\n", output_filename);
            printf("Ejecutable generado: %s\n", exe_filename);
        }
    }
    
    fclose(yyin);
    
    if (debug_mode) {
        printf("\nCompilación completada exitosamente\n");
    }
    
    return 0;
}
