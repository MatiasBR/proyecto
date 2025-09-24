%{
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

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

%type <node> program method_decl block statement expr
%type <node> method_decl_list statement_list

%start program

%%

program:
    PROGRAM LBRACE method_decl_list RBRACE {
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

method_decl_list:
    /* empty */ {
        $$ = create_ast_node(METHOD_DECL_LIST_NODE, 0, NULL);
        if ($$) {
            $$->children = NULL;
            $$->child_count = 0;
        }
    }
    | method_decl_list method_decl {
        $$ = $1;
        if ($$ && $2) {
            $$->children = realloc($$->children, sizeof(ASTNode*) * ($$->child_count + 1));
            if ($$->children) {
                $$->children[$$->child_count] = $2;
                $$->child_count++;
            }
        }
    }
    ;

method_decl:
    VOID IDENTIFIER LPAREN RPAREN block {
        $$ = create_ast_node(METHOD_DECL_NODE, 0, $2);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*));
            if ($$->children) {
                $$->children[0] = $5;
                $$->child_count = 1;
            }
        }
    }
    ;

block:
    LBRACE statement_list RBRACE {
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

statement_list:
    /* empty */ {
        $$ = create_ast_node(STATEMENT_LIST_NODE, 0, NULL);
        if ($$) {
            $$->children = NULL;
            $$->child_count = 0;
        }
    }
    | statement_list statement {
        $$ = $1;
        if ($$ && $2) {
            $$->children = realloc($$->children, sizeof(ASTNode*) * ($$->child_count + 1));
            if ($$->children) {
                $$->children[$$->child_count] = $2;
                $$->child_count++;
            }
        }
    }
    ;

statement:
    RETURN SEMICOLON {
        $$ = create_ast_node(RETURN_STMT_NODE, 0, NULL);
        if ($$) {
            $$->children = NULL;
            $$->child_count = 0;
        }
    }
    | block {
        $$ = $1;
    }
    ;

expr:
    IDENTIFIER {
        $$ = create_ast_node(IDENTIFIER_NODE, 0, $1);
        if ($$) {
            $$->children = NULL;
            $$->child_count = 0;
        }
    }
    | INTEGER_LITERAL {
        $$ = create_ast_node(INTEGER_LITERAL_NODE, $1, NULL);
        if ($$) {
            $$->children = NULL;
            $$->child_count = 0;
        }
    }
    | TRUE {
        $$ = create_ast_node(BOOL_LITERAL_NODE, 1, NULL);
        if ($$) {
            $$->children = NULL;
            $$->child_count = 0;
        }
    }
    | FALSE {
        $$ = create_ast_node(BOOL_LITERAL_NODE, 0, NULL);
        if ($$) {
            $$->children = NULL;
            $$->child_count = 0;
        }
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error sintáctico en línea %d: %s\n", yylineno, s);
}
