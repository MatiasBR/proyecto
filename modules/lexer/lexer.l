%{
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ast/ast.h"
#include "../parser/parser.h"
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

[0-9]+ {
    yylval.int_val = atoi(yytext);
    return INTEGER_LITERAL;
}

"//".*$          { /* Comentario de línea - ignorar */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /* Comentario multilínea - ignorar */ }

[ \t\n\r]+       { /* Espacios en blanco - ignorar */ }

.                { 
    fprintf(stderr, "Error léxico: carácter no reconocido '%c' en línea %d\n", yytext[0], yylineno);
    return ERROR;
}

%%
