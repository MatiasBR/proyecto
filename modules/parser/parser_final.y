%{
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ast/ast.h"

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
%token PROGRAM BOOL ELSE EXTERN FALSE IF INTEGER RETURN THEN TRUE VOID WHILE
%token PLUS MINUS MULTIPLY DIVIDE MODULO LESS GREATER EQUAL AND OR NOT ASSIGN
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA ERROR

%type <node> program method_decl block statement_list statement

%start program

%%

program:
    PROGRAM LBRACE method_decl RBRACE {
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

method_decl:
    VOID IDENTIFIER LPAREN RPAREN block {
        $$ = create_ast_node(METHOD_DECL_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = create_ast_node(IDENTIFIER_NODE, 0, strdup($2));
                $$->children[1] = NULL;  // param_list
                $$->children[2] = $5;     // block
                $$->child_count = 3;
            }
        }
    }
    ;

block:
    LBRACE statement_list RBRACE {
        $$ = create_ast_node(BLOCK_NODE, 0, NULL);
        if ($$) {
            if ($2) {
                $$->children = malloc(sizeof(ASTNode*));
                if ($$->children) {
                    $$->children[0] = $2;
                    $$->child_count = 1;
                } else {
                    $$->child_count = 0;
                }
            } else {
                $$->children = NULL;
                $$->child_count = 0;
            }
        }
    }
    ;

statement_list:
    /* empty */ {
        $$ = NULL;
    }
    | statement_list statement {
        if ($1 && $2) {
            if ($1->children) {
                $1->children = realloc($1->children, sizeof(ASTNode*) * ($1->child_count + 1));
                if ($1->children) {
                    $1->children[$1->child_count] = $2;
                    $1->child_count++;
                }
            } else {
                $1->children = malloc(sizeof(ASTNode*));
                if ($1->children) {
                    $1->children[0] = $2;
                    $1->child_count = 1;
                }
            }
        }
        $$ = $1;
    }
    ;

statement:
    RETURN SEMICOLON {
        $$ = create_ast_node(RETURN_NODE, 0, NULL);
        if ($$) {
            $$->children = NULL;
            $$->child_count = 0;
        }
    }
    | block {
        $$ = $1;
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error sintáctico en línea %d: %s\n", yylineno, s);
}
