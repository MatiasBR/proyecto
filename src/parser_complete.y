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
%token PROGRAM BOOL ELSE EXTERN FALSE IF INTEGER RETURN THEN TRUE VOID WHILE
%token PLUS MINUS MULTIPLY DIVIDE MODULO LESS GREATER EQUAL AND OR NOT ASSIGN
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA ERROR

%type <node> program var_decl_list var_decl method_decl_list method_decl extern_method_decl
%type <node> param_list param block statement_list statement
%type <node> assign_stmt method_call_stmt if_stmt while_stmt return_stmt empty_stmt
%type <node> expression binary_op unary_op arith_op rel_op cond_op
%type <node> literal integer_literal bool_literal type integer_type bool_type void_type
%type <node> arg_list method_call

%start program

%%

program:
    PROGRAM LBRACE var_decl_list method_decl_list RBRACE {
        $$ = create_ast_node(PROGRAM_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(2 * sizeof(ASTNode*));
            $$->children[0] = $3;
            $$->children[1] = $4;
            $$->child_count = 2;
        }
        root = $$;
    }
    ;

var_decl_list:
    /* empty */ {
        $$ = create_ast_node(VAR_DECL_LIST_NODE, 0, NULL);
        if ($$) {
            $$->children = NULL;
            $$->child_count = 0;
        }
    }
    | var_decl_list var_decl {
        if ($1) {
            $1->children = realloc($1->children, ($1->child_count + 1) * sizeof(ASTNode*));
            $1->children[$1->child_count] = $2;
            $1->child_count++;
        }
        $$ = $1;
    }
    ;

var_decl:
    type IDENTIFIER ASSIGN expression SEMICOLON {
        $$ = create_ast_node(VAR_DECL_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(3 * sizeof(ASTNode*));
            $$->children[0] = $1;
            $$->children[1] = create_ast_node(IDENTIFIER_NODE, 0, $2);
            $$->children[2] = $4;
            $$->child_count = 3;
        }
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
        if ($1) {
            $1->children = realloc($1->children, ($1->child_count + 1) * sizeof(ASTNode*));
            $1->children[$1->child_count] = $2;
            $1->child_count++;
        }
        $$ = $1;
    }
    | method_decl_list extern_method_decl {
        if ($1) {
            $1->children = realloc($1->children, ($1->child_count + 1) * sizeof(ASTNode*));
            $1->children[$1->child_count] = $2;
            $1->child_count++;
        }
        $$ = $1;
    }
    ;

method_decl:
    type IDENTIFIER LPAREN param_list RPAREN block {
        $$ = create_ast_node(METHOD_DECL_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(3 * sizeof(ASTNode*));
            $$->children[0] = create_ast_node(IDENTIFIER_NODE, 0, $2);
            $$->children[1] = $4;
            $$->children[2] = $6;
            $$->child_count = 3;
        }
    }
    ;

extern_method_decl:
    type IDENTIFIER LPAREN param_list RPAREN EXTERN SEMICOLON {
        $$ = create_ast_node(EXTERN_METHOD_DECL_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(2 * sizeof(ASTNode*));
            $$->children[0] = create_ast_node(IDENTIFIER_NODE, 0, $2);
            $$->children[1] = $4;
            $$->child_count = 2;
        }
    }
    ;

param_list:
    /* empty */ {
        $$ = create_ast_node(PARAM_LIST_NODE, 0, NULL);
        if ($$) {
            $$->children = NULL;
            $$->child_count = 0;
        }
    }
    | param_list COMMA param {
        if ($1) {
            $1->children = realloc($1->children, ($1->child_count + 1) * sizeof(ASTNode*));
            $1->children[$1->child_count] = $3;
            $1->child_count++;
        }
        $$ = $1;
    }
    | param {
        $$ = create_ast_node(PARAM_LIST_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*));
            $$->children[0] = $1;
            $$->child_count = 1;
        }
    }
    ;

param:
    type IDENTIFIER {
        $$ = create_ast_node(PARAM_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(2 * sizeof(ASTNode*));
            $$->children[0] = $1;
            $$->children[1] = create_ast_node(IDENTIFIER_NODE, 0, $2);
            $$->child_count = 2;
        }
    }
    ;

block:
    LBRACE var_decl_list statement_list RBRACE {
        $$ = create_ast_node(BLOCK_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(2 * sizeof(ASTNode*));
            $$->children[0] = $2;
            $$->children[1] = $3;
            $$->child_count = 2;
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
        if ($1) {
            $1->children = realloc($1->children, ($1->child_count + 1) * sizeof(ASTNode*));
            $1->children[$1->child_count] = $2;
            $1->child_count++;
        }
        $$ = $1;
    }
    ;

statement:
    assign_stmt {
        $$ = $1;
    }
    | method_call_stmt {
        $$ = $1;
    }
    | if_stmt {
        $$ = $1;
    }
    | while_stmt {
        $$ = $1;
    }
    | return_stmt {
        $$ = $1;
    }
    | empty_stmt {
        $$ = $1;
    }
    | block {
        $$ = $1;
    }
    ;

assign_stmt:
    IDENTIFIER ASSIGN expression SEMICOLON {
        $$ = create_ast_node(ASSIGN_STMT_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(2 * sizeof(ASTNode*));
            $$->children[0] = create_ast_node(IDENTIFIER_NODE, 0, $1);
            $$->children[1] = $3;
            $$->child_count = 2;
        }
    }
    ;

method_call_stmt:
    IDENTIFIER LPAREN arg_list RPAREN SEMICOLON {
        $$ = create_ast_node(METHOD_CALL_STMT_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(2 * sizeof(ASTNode*));
            $$->children[0] = create_ast_node(IDENTIFIER_NODE, 0, $1);
            $$->children[1] = $3;
            $$->child_count = 2;
        }
    }
    ;

if_stmt:
    IF LPAREN expression RPAREN THEN block ELSE block {
        $$ = create_ast_node(IF_STMT_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(3 * sizeof(ASTNode*));
            $$->children[0] = $3;
            $$->children[1] = $6;
            $$->children[2] = $8;
            $$->child_count = 3;
        }
    }
    ;

while_stmt:
    WHILE LPAREN expression RPAREN block {
        $$ = create_ast_node(WHILE_STMT_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(2 * sizeof(ASTNode*));
            $$->children[0] = $3;
            $$->children[1] = $5;
            $$->child_count = 2;
        }
    }
    ;

return_stmt:
    RETURN expression SEMICOLON {
        $$ = create_ast_node(RETURN_STMT_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*));
            $$->children[0] = $2;
            $$->child_count = 1;
        }
    }
    | RETURN SEMICOLON {
        $$ = create_ast_node(RETURN_STMT_NODE, 0, NULL);
        if ($$) {
            $$->children = NULL;
            $$->child_count = 0;
        }
    }
    ;

empty_stmt:
    SEMICOLON {
        $$ = create_ast_node(EMPTY_STMT_NODE, 0, NULL);
        if ($$) {
            $$->children = NULL;
            $$->child_count = 0;
        }
    }
    ;

expression:
    IDENTIFIER {
        $$ = create_ast_node(IDENTIFIER_NODE, 0, $1);
    }
    | method_call {
        $$ = $1;
    }
    | literal {
        $$ = $1;
    }
    | binary_op {
        $$ = $1;
    }
    | unary_op {
        $$ = $1;
    }
    | LPAREN expression RPAREN {
        $$ = $2;
    }
    ;

method_call:
    IDENTIFIER LPAREN arg_list RPAREN {
        $$ = create_ast_node(METHOD_CALL_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(2 * sizeof(ASTNode*));
            $$->children[0] = create_ast_node(IDENTIFIER_NODE, 0, $1);
            $$->children[1] = $3;
            $$->child_count = 2;
        }
    }
    ;

binary_op:
    expression arith_op expression {
        $$ = create_ast_node(ARITH_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(3 * sizeof(ASTNode*));
            $$->children[0] = $1;
            $$->children[1] = $2;
            $$->children[2] = $3;
            $$->child_count = 3;
        }
    }
    | expression rel_op expression {
        $$ = create_ast_node(REL_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(3 * sizeof(ASTNode*));
            $$->children[0] = $1;
            $$->children[1] = $2;
            $$->children[2] = $3;
            $$->child_count = 3;
        }
    }
    | expression cond_op expression {
        $$ = create_ast_node(COND_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(3 * sizeof(ASTNode*));
            $$->children[0] = $1;
            $$->children[1] = $2;
            $$->children[2] = $3;
            $$->child_count = 3;
        }
    }
    ;

unary_op:
    MINUS expression {
        $$ = create_ast_node(UNARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(2 * sizeof(ASTNode*));
            $$->children[0] = create_ast_node(OPERATOR_NODE, 0, "-");
            $$->children[1] = $2;
            $$->child_count = 2;
        }
    }
    | NOT expression {
        $$ = create_ast_node(UNARY_OP_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(2 * sizeof(ASTNode*));
            $$->children[0] = create_ast_node(OPERATOR_NODE, 0, "!");
            $$->children[1] = $2;
            $$->child_count = 2;
        }
    }
    ;

arith_op:
    PLUS {
        $$ = create_ast_node(OPERATOR_NODE, 0, "+");
    }
    | MINUS {
        $$ = create_ast_node(OPERATOR_NODE, 0, "-");
    }
    | MULTIPLY {
        $$ = create_ast_node(OPERATOR_NODE, 0, "*");
    }
    | DIVIDE {
        $$ = create_ast_node(OPERATOR_NODE, 0, "/");
    }
    | MODULO {
        $$ = create_ast_node(OPERATOR_NODE, 0, "%");
    }
    ;

rel_op:
    LESS {
        $$ = create_ast_node(OPERATOR_NODE, 0, "<");
    }
    | GREATER {
        $$ = create_ast_node(OPERATOR_NODE, 0, ">");
    }
    | EQUAL {
        $$ = create_ast_node(OPERATOR_NODE, 0, "==");
    }
    ;

cond_op:
    AND {
        $$ = create_ast_node(OPERATOR_NODE, 0, "&&");
    }
    | OR {
        $$ = create_ast_node(OPERATOR_NODE, 0, "||");
    }
    ;

literal:
    integer_literal {
        $$ = $1;
    }
    | bool_literal {
        $$ = $1;
    }
    ;

integer_literal:
    INTEGER_LITERAL {
        $$ = create_ast_node(INTEGER_LITERAL_NODE, $1, NULL);
    }
    ;

bool_literal:
    TRUE {
        $$ = create_ast_node(BOOL_LITERAL_NODE, 1, NULL);
    }
    | FALSE {
        $$ = create_ast_node(BOOL_LITERAL_NODE, 0, NULL);
    }
    ;

type:
    integer_type {
        $$ = $1;
    }
    | bool_type {
        $$ = $1;
    }
    | void_type {
        $$ = $1;
    }
    ;

integer_type:
    INTEGER {
        $$ = create_ast_node(INTEGER_TYPE_NODE, 0, NULL);
    }
    ;

bool_type:
    BOOL {
        $$ = create_ast_node(BOOL_TYPE_NODE, 0, NULL);
    }
    ;

void_type:
    VOID {
        $$ = create_ast_node(VOID_TYPE_NODE, 0, NULL);
    }
    ;

arg_list:
    /* empty */ {
        $$ = create_ast_node(ARG_LIST_NODE, 0, NULL);
        if ($$) {
            $$->children = NULL;
            $$->child_count = 0;
        }
    }
    | arg_list COMMA expression {
        if ($1) {
            $1->children = realloc($1->children, ($1->child_count + 1) * sizeof(ASTNode*));
            $1->children[$1->child_count] = $3;
            $1->child_count++;
        }
        $$ = $1;
    }
    | expression {
        $$ = create_ast_node(ARG_LIST_NODE, 0, NULL);
        if ($$) {
            $$->children = malloc(sizeof(ASTNode*));
            $$->children[0] = $1;
            $$->child_count = 1;
        }
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error sintáctico en línea %d: %s\n", yylineno, s);
}
