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

%type <node> program var_decl_list method_decl_list var_decl method_decl
%type <node> type_or_void param_list block statement_list type statement
%type <node> else_part expr_opt method_call arg_list expr
%type <node> literal

%start program

%%

program:
    PROGRAM LBRACE var_decl_list method_decl_list RBRACE {
        $$ = create_ast_node(PROGRAM_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 2);
            if ($$->children) {
                $$->children[0] = $3;  // var_decl_list (puede ser NULL)
                $$->children[1] = $4;  // method_decl_list (puede ser NULL)
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
        if ($1) {
            $1->children = realloc($1->children, sizeof(ASTNode*) * ($1->child_count + 1));
            if ($1->children) {
                $1->children[$1->child_count] = $2;
                $1->child_count++;
            }
            $$ = $1;
        } else {
            $$ = create_ast_node(VAR_DECL_LIST_NODE, 0, NULL);
            if ($$) {
                $$->children = malloc(sizeof(ASTNode*));
                if ($$->children) {
                    $$->children[0] = $2;
                    $$->child_count = 1;
                }
            }
        }
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
            $$ = $1;
        } else if ($2) {
            $$ = create_ast_node(METHOD_DECL_LIST_NODE, 0, NULL);
            if ($$) {
                $$->children = malloc(sizeof(ASTNode*));
                if ($$->children) {
                    $$->children[0] = $2;
                    $$->child_count = 1;
                }
            }
        } else {
            $$ = NULL;
        }
    }
    ;

var_decl:
    type IDENTIFIER SEMICOLON {
        $$ = create_ast_node(VAR_DECL_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 2);
            if ($$->children) {
                $$->children[0] = $1;  // type
                $$->children[1] = create_ast_node(IDENTIFIER_NODE, 0, strdup($2));  // IDENTIFIER
                $$->child_count = 2;
            }
        }
    }
    ;

method_decl:
    type_or_void IDENTIFIER LPAREN param_list RPAREN block {
        $$ = create_ast_node(METHOD_DECL_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 4);
            if ($$->children) {
                $$->children[0] = $1;  // type_or_void
                $$->children[1] = create_ast_node(IDENTIFIER_NODE, 0, strdup($2));  // IDENTIFIER
                $$->children[2] = $4;  // param_list (puede ser NULL)
                $$->children[3] = $6;  // block
                $$->child_count = 4;
            }
        }
    }
    ;

type_or_void:
    type {
        $$ = $1;
    }
    | VOID {
        $$ = create_ast_node(TYPE_NODE, 0, strdup("void"));
    }
    ;

param_list:
    /* empty */ {
        $$ = NULL;
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

type:
    INTEGER {
        $$ = create_ast_node(TYPE_NODE, 0, strdup("integer"));
    }
    | BOOL {
        $$ = create_ast_node(TYPE_NODE, 0, strdup("bool"));
    }
    ;

statement:
    var_decl {
        $$ = $1;
    }
    | RETURN SEMICOLON {
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
