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
%token PROGRAM BOOL DO ELSE EXTERN FALSE IF INTEGER RETURN THEN TRUE VOID WHILE
%token PLUS MINUS MULTIPLY DIVIDE MODULO LESS GREATER EQUAL AND OR NOT ASSIGN
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA ERROR

%type <node> program var_decl_list method_decl_list var_decl method_decl
%type <node> type statement expr

%start program

%%

program:
    PROGRAM LBRACE var_decl_list method_decl_list RBRACE {
        $$ = create_ast_node(PROGRAM_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 2);
            if ($$->children) {
                $$->children[0] = $3;
                $$->children[1] = $4;
                $$->child_count = 2;
            }
        }
        root = $$;
    }
    ;

var_decl_list:
    /* empty */ {
        $$ = NULL;
    }
    | var_decl_list var_decl {
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

method_decl_list:
    /* empty */ {
        $$ = NULL;
    }
    | method_decl_list method_decl {
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

var_decl:
    type IDENTIFIER SEMICOLON {
        $$ = create_ast_node(VAR_DECL_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 2);
            if ($$->children) {
                $$->children[0] = $1;
                $$->children[1] = create_ast_node(IDENTIFIER_NODE, 0, strdup($2));
                $$->child_count = 2;
            }
        }
    }
    ;

method_decl:
    VOID IDENTIFIER LPAREN RPAREN LBRACE statement RBRACE {
        $$ = create_ast_node(METHOD_DECL_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = create_ast_node(IDENTIFIER_NODE, 0, strdup($2));
                $$->children[1] = NULL;
                $$->children[2] = $6;
                $$->child_count = 3;
            }
        }
    }
    ;

type:
    INTEGER {
        $$ = create_ast_node(TYPE_NODE, 0, strdup("integer"));
    }
    | BOOL {
        $$ = create_ast_node(TYPE_NODE, 0, strdup("bool"));
    }
    ;

statement:
    IDENTIFIER ASSIGN expr SEMICOLON {
        $$ = create_ast_node(ASSIGN_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 2);
            if ($$->children) {
                $$->children[0] = create_ast_node(IDENTIFIER_NODE, 0, strdup($1));
                $$->children[1] = $3;
                $$->child_count = 2;
            }
        }
    }
    | RETURN expr SEMICOLON {
        $$ = create_ast_node(RETURN_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*));
            if ($$->children) {
                $$->children[0] = $2;
                $$->child_count = 1;
            }
        }
    }
    | /* empty */ {
        $$ = NULL;
    }
    ;

expr:
    INTEGER_LITERAL {
        $$ = create_ast_node(INTEGER_LITERAL_NODE, $1, NULL);
    }
    | IDENTIFIER {
        $$ = create_ast_node(IDENTIFIER_NODE, 0, strdup($1));
    }
    | expr PLUS expr {
        $$ = create_ast_node(BINARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = $1;
                $$->children[1] = create_ast_node(OPERATOR_NODE, 0, strdup("+"));
                $$->children[2] = $3;
                $$->child_count = 3;
            }
        }
    }
    | LPAREN expr RPAREN {
        $$ = $2;
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error sintáctico en línea %d: %s\n", yylineno, s);
}
