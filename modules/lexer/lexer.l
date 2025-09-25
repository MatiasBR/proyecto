%{
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>
#include "../ast/ast.h"
#include "../parser/parser.h"

extern YYSTYPE yylval;

void reportar_error(const char *msg, const char *text, int lineno);
%}

%option noyywrap
%option yylineno

%%

"program"        { return PROGRAM; }
"bool"           { return BOOL; }
"else"           { return ELSE; }
"extern"         { return EXTERN; }
"false"          { return FALSE; }
"if"             { return IF; }
"integer"        { return INTEGER; }
"return"         { return RETURN; }
"then"           { return THEN; }
"true"           { return TRUE; }
"void"           { return VOID; }
"while"          { return WHILE; }

"+"              { return PLUS; }
"-"              { return MINUS; }
"*"              { return MULTIPLY; }
"/"              { return DIVIDE; }
"%"              { return MODULO; }
"<"              { return LESS; }
">"              { return GREATER; }
"=="             { return EQUAL; }
"&&"             { return AND; }
"||"             { return OR; }
"!"              { return NOT; }
"="              { return ASSIGN; }

"("              { return LPAREN; }
")"              { return RPAREN; }
"{"              { return LBRACE; }
"}"              { return RBRACE; }
";"              { return SEMICOLON; }
","              { return COMMA; }

[a-zA-Z][a-zA-Z0-9_]* {
    yylval.string_val = strdup(yytext);
    return IDENTIFIER;
}

[-]?[0-9]+ {
    errno = 0;
    long long val = strtol(yytext, NULL, 10);
    
    if (errno == ERANGE || val < INT32_MIN || val > INT32_MAX) {
        reportar_error("Literal entero fuera de rango (se espera int32 con signo)", yytext, yylineno);
    } else {
        yylval.int_val = (int)val;
        return INTEGER_LITERAL;
    }
}

"//".*$          { /* Comentario de línea - ignorar */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /* Comentario multilínea - ignorar */ }

[ \t\n\r]+       { /* Espacios en blanco - ignorar */ }

.                { 
    reportar_error("Caracter inesperado", yytext, yylineno);
}

%%

/* Función para reportar un error */
void reportar_error(const char *msg, const char *text, int lineno) {
    fprintf(stderr, "Error léxico en línea %d: %s: '%s'\n", lineno, msg, text);
}
