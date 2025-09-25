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

/* Habilitar mensajes de error detallados */
%define parse.error verbose

%union {
    int int_val;
    char* string_val;
    ASTNode* node;
}

%token <int_val> INTEGER_LITERAL
%token <string_val> IDENTIFIER
%token PROGRAM BOOL DO ELSE EXTERN FALSE IF INTEGER RETURN THEN TRUE VOID WHILE
%token PLUS MINUS MULTIPLY DIVIDE MODULO LESS GREATER EQUAL AND OR NOT ASSIGN
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA ERROR UMINUS

%left OR
%left AND
%left EQUAL
%left LESS GREATER
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT
%right UMINUS

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
            // Agregar a lista existente
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
            // Crear nueva lista
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
    | type IDENTIFIER {
        $$ = create_ast_node(PARAM_LIST_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 2);
            if ($$->children) {
                $$->children[0] = $1;
                $$->children[1] = create_ast_node(IDENTIFIER_NODE, 0, strdup($2));
                $$->child_count = 2;
            }
        }
    }
    | param_list COMMA type IDENTIFIER {
        if ($1 && $3 && $4) {
            if ($1->children) {
                $1->children = realloc($1->children, sizeof(ASTNode*) * ($1->child_count + 2));
                if ($1->children) {
                    $1->children[$1->child_count] = $3;
                    $1->children[$1->child_count + 1] = create_ast_node(IDENTIFIER_NODE, 0, strdup($4));
                    $1->child_count += 2;
                }
            }
        }
        $$ = $1;
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
    | IDENTIFIER ASSIGN expr SEMICOLON {
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
    | method_call SEMICOLON {
        $$ = $1;
    }
    | IF expr THEN block else_part {
        $$ = create_ast_node(IF_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = $2;
                $$->children[1] = $4;
                $$->children[2] = $5;
                $$->child_count = 3;
            }
        }
    }
    | WHILE expr block {
        $$ = create_ast_node(WHILE_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 2);
            if ($$->children) {
                $$->children[0] = $2;
                $$->children[1] = $3;
                $$->child_count = 2;
            }
        }
    }
    | RETURN expr_opt SEMICOLON {
        $$ = create_ast_node(RETURN_NODE, 0, NULL);
        if ($$) {
            $$->children = $2 ? malloc(sizeof(ASTNode*)) : NULL;
            if ($$->children && $2) {
                $$->children[0] = $2;
                $$->child_count = 1;
            }
        }
    }
    | block {
        $$ = $1;
    }
    ;

else_part:
    /* empty */ {
        $$ = NULL;
    }
    | ELSE block {
        $$ = $2;
    }
    ;

expr_opt:
    /* empty */ {
        $$ = NULL;
    }
    | expr {
        $$ = $1;
    }
    ;

method_call:
    IDENTIFIER LPAREN arg_list RPAREN {
        $$ = create_ast_node(METHOD_CALL_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 2);
            if ($$->children) {
                $$->children[0] = create_ast_node(IDENTIFIER_NODE, 0, strdup($1));
                $$->children[1] = $3;
                $$->child_count = 2;
            }
        }
    }
    ;

arg_list:
    /* empty */ {
        $$ = NULL;
    }
    | expr {
        $$ = create_ast_node(ARG_LIST_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*));
            if ($$->children) {
                $$->children[0] = $1;
                $$->child_count = 1;
            }
        }
    }
    | arg_list COMMA expr {
        if ($1 && $3) {
            if ($1->children) {
                $1->children = realloc($1->children, sizeof(ASTNode*) * ($1->child_count + 1));
                if ($1->children) {
                    $1->children[$1->child_count] = $3;
                    $1->child_count++;
                }
            }
        }
        $$ = $1;
    }
    ;

expr:
    literal {
        $$ = $1;
    }
    | IDENTIFIER {
        $$ = create_ast_node(IDENTIFIER_NODE, 0, strdup($1));
    }
    | method_call {
        $$ = $1;
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
    | expr MINUS expr {
        $$ = create_ast_node(BINARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = $1;
                $$->children[1] = create_ast_node(OPERATOR_NODE, 0, strdup("-"));
                $$->children[2] = $3;
                $$->child_count = 3;
            }
        }
    }
    | expr MULTIPLY expr {
        $$ = create_ast_node(BINARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = $1;
                $$->children[1] = create_ast_node(OPERATOR_NODE, 0, strdup("*"));
                $$->children[2] = $3;
                $$->child_count = 3;
            }
        }
    }
    | expr DIVIDE expr {
        $$ = create_ast_node(BINARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = $1;
                $$->children[1] = create_ast_node(OPERATOR_NODE, 0, strdup("/"));
                $$->children[2] = $3;
                $$->child_count = 3;
            }
        }
    }
    | expr MODULO expr {
        $$ = create_ast_node(BINARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = $1;
                $$->children[1] = create_ast_node(OPERATOR_NODE, 0, strdup("%"));
                $$->children[2] = $3;
                $$->child_count = 3;
            }
        }
    }
    | expr LESS expr {
        $$ = create_ast_node(BINARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = $1;
                $$->children[1] = create_ast_node(OPERATOR_NODE, 0, strdup("<"));
                $$->children[2] = $3;
                $$->child_count = 3;
            }
        }
    }
    | expr GREATER expr {
        $$ = create_ast_node(BINARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = $1;
                $$->children[1] = create_ast_node(OPERATOR_NODE, 0, strdup(">"));
                $$->children[2] = $3;
                $$->child_count = 3;
            }
        }
    }
    | expr EQUAL expr {
        $$ = create_ast_node(BINARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = $1;
                $$->children[1] = create_ast_node(OPERATOR_NODE, 0, strdup("=="));
                $$->children[2] = $3;
                $$->child_count = 3;
            }
        }
    }
    | expr AND expr {
        $$ = create_ast_node(BINARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = $1;
                $$->children[1] = create_ast_node(OPERATOR_NODE, 0, strdup("&&"));
                $$->children[2] = $3;
                $$->child_count = 3;
            }
        }
    }
    | expr OR expr {
        $$ = create_ast_node(BINARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 3);
            if ($$->children) {
                $$->children[0] = $1;
                $$->children[1] = create_ast_node(OPERATOR_NODE, 0, strdup("||"));
                $$->children[2] = $3;
                $$->child_count = 3;
            }
        }
    }
    | MINUS expr %prec UMINUS {
        $$ = create_ast_node(UNARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 2);
            if ($$->children) {
                $$->children[0] = create_ast_node(OPERATOR_NODE, 0, strdup("-"));
                $$->children[1] = $2;
                $$->child_count = 2;
            }
        }
    }
    | NOT expr {
        $$ = create_ast_node(UNARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*) * 2);
            if ($$->children) {
                $$->children[0] = create_ast_node(OPERATOR_NODE, 0, strdup("!"));
                $$->children[1] = $2;
                $$->child_count = 2;
            }
        }
    }
    | LPAREN expr RPAREN {
        $$ = $2;
    }
    ;


literal:
    INTEGER_LITERAL {
        $$ = create_ast_node(INTEGER_LITERAL_NODE, $1, NULL);
    }
    | TRUE {
        $$ = create_ast_node(BOOLEAN_LITERAL_NODE, 1, NULL);
    }
    | FALSE {
        $$ = create_ast_node(BOOLEAN_LITERAL_NODE, 0, NULL);
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error sintáctico en línea %d: %s\n", yylineno, s);
}
