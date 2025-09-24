%{
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "modules/ast/ast.h"

extern int yylineno;
extern char* yytext;
extern FILE* yyin;

int yylex(void);
void yyerror(const char* s);

ASTNode* root = NULL;
%}

%union {
    int int_val;
    char* string_val;
    ASTNode* node;
}

%token <int_val> INTEGER_LITERAL
%token <string_val> IDENTIFIER
%token PROGRAM BOOL DO ELSE EXTERN FALSE IF INTEGER RETURN THEN TRUE VOID WHILE
%token PLUS MINUS MULTIPLY DIVIDE MODULO LESS GREATER EQUAL AND OR NOT ASSIGN
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA ERROR

%type <node> program block statement function_decl

%start program

%%

program:
    PROGRAM LBRACE function_decl RBRACE {
        $$ = create_ast_node(PROGRAM_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*));
            if ($$->children) {
                $$->children[0] = $3;
                $$->child_count = 1;
            }
        }
        root = $$;
    }
    ;

block:
    LBRACE RBRACE {
        $$ = create_ast_node(BLOCK_NODE, 0, NULL);
        $$->children = NULL;
        $$->child_count = 0;
    }
    | LBRACE statement RBRACE {
        $$ = create_ast_node(BLOCK_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*));
            if ($$->children) {
                $$->children[0] = $2;
                $$->child_count = 1;
            }
        }
    }
    ;

function_decl:
    VOID IDENTIFIER LPAREN RPAREN block {
        $$ = create_ast_node(METHOD_DECL_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = create_ast_node(TYPE_NODE, 0, strdup("void"));
                $$->children[1] = create_ast_node(IDENTIFIER_NODE, 0, strdup($2));
                $$->children[2] = $5;
                $$->child_count = 3;
            }
        }
    }
    ;

statement:
    RETURN SEMICOLON {
        $$ = create_ast_node(RETURN_NODE, 0, NULL);
        $$->children = NULL;
        $$->child_count = 0;
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error sintáctico en línea %d: %s\n", yylineno, s);
}
