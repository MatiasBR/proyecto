#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "modules/ast/ast.h"

extern FILE* yyin;
extern int yyparse(void);
extern ASTNode* root;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Uso: %s archivo.ctds\n", argv[0]);
        return 1;
    }
    
    char* input_file = argv[1];
    
    // Abrir archivo de entrada
    yyin = fopen(input_file, "r");
    if (!yyin) {
        fprintf(stderr, "Error: No se pudo abrir el archivo '%s'\n", input_file);
        return 1;
    }
    
    printf("Iniciando análisis sintáctico...\n");
    
    int parse_result = yyparse();
    printf("Resultado del parse: %d\n", parse_result);
    
    if (parse_result != 0) {
        fprintf(stderr, "Error en el análisis sintáctico\n");
        fclose(yyin);
        return 1;
    }
    
    printf("Análisis sintáctico exitoso\n");
    
    if (root) {
        printf("AST creado exitosamente\n");
    } else {
        printf("ERROR: AST es NULL\n");
    }
    
    fclose(yyin);
    return 0;
}
