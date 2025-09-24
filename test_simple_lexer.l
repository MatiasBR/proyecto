%{
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "modules/ast/ast.h"
#include "test_simple_parser.h"
%}

%option noyywrap
%option yylineno

%%

"program"        { printf("LEXER: PROGRAM\n"); return PROGRAM; }
"void"           { printf("LEXER: VOID\n"); return VOID; }
"return"         { printf("LEXER: RETURN\n"); return RETURN; }

"("              { printf("LEXER: LPAREN\n"); return LPAREN; }
")"              { printf("LEXER: RPAREN\n"); return RPAREN; }
"{"              { printf("LEXER: LBRACE\n"); return LBRACE; }
"}"              { printf("LEXER: RBRACE\n"); return RBRACE; }
";"              { printf("LEXER: SEMICOLON\n"); return SEMICOLON; }

[a-zA-Z][a-zA-Z0-9_]* {
    printf("LEXER: Identificador '%s'\n", yytext);
    yylval.string_val = strdup(yytext);
    return IDENTIFIER;
}

"//".*$          { /* Comentario de línea - ignorar */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /* Comentario multilínea - ignorar */ }

[ \t\n\r]+       { /* Espacios en blanco - ignorar */ }

.                { 
    fprintf(stderr, "Error léxico: carácter no reconocido '%c' en línea %d\n", yytext[0], yylineno);
    return ERROR;
}

%%
