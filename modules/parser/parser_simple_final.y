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

%type <node> program var_decl_list var_decl method_decl_list method_decl
%type <node> type param_list param block statement_list statement
%type <node> expr bin_op arith_op rel_op cond_op literal
%type <node> method_call arg_list

%start program

%left OR
%left AND
%left EQUAL
%left LESS GREATER
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT
%right UMINUS

%%

program:
    PROGRAM LBRACE var_decl_list method_decl_list RBRACE {
        $$ = create_ast_node(PROGRAM_NODE, 0, NULL);
        if ($3) {
            for (int i = 0; i < $3->child_count; i++) {
                add_child($$, $3->children[i]);
            }
            free($3->children);
            free($3);
        }
        if ($4) {
            for (int i = 0; i < $4->child_count; i++) {
                add_child($$, $4->children[i]);
            }
            free($4->children);
            free($4);
        }
        root = $$;
    }
    ;

var_decl_list:
    /* empty */ {
        $$ = create_list_node(VAR_DECL_LIST_NODE);
    }
    | var_decl_list var_decl {
        add_to_list($1, $2);
        $$ = $1;
    }
    ;

var_decl:
    type IDENTIFIER ASSIGN expr SEMICOLON {
        $$ = create_ast_node(VAR_DECL_NODE, 0, $2);
        add_child($$, $1);
        add_child($$, $4);
    }
    ;

method_decl_list:
    /* empty */ {
        $$ = create_list_node(METHOD_DECL_LIST_NODE);
    }
    | method_decl_list method_decl {
        add_to_list($1, $2);
        $$ = $1;
    }
    ;

method_decl:
    type IDENTIFIER LPAREN param_list RPAREN block {
        $$ = create_ast_node(METHOD_DECL_NODE, 0, $2);
        add_child($$, $1);
        if ($4) add_child($$, $4);
        add_child($$, $6);
    }
    | VOID IDENTIFIER LPAREN param_list RPAREN block {
        $$ = create_ast_node(METHOD_DECL_NODE, 0, $2);
        add_child($$, create_ast_node(VOID_TYPE_NODE, 0, NULL));
        if ($4) add_child($$, $4);
        add_child($$, $6);
    }
    | type IDENTIFIER LPAREN param_list RPAREN EXTERN SEMICOLON {
        $$ = create_ast_node(EXTERN_METHOD_DECL_NODE, 0, $2);
        add_child($$, $1);
        if ($4) add_child($$, $4);
    }
    | VOID IDENTIFIER LPAREN param_list RPAREN EXTERN SEMICOLON {
        $$ = create_ast_node(EXTERN_METHOD_DECL_NODE, 0, $2);
        add_child($$, create_ast_node(VOID_TYPE_NODE, 0, NULL));
        if ($4) add_child($$, $4);
    }
    ;

param_list:
    /* empty */ {
        $$ = NULL;
    }
    | param {
        $$ = create_list_node(PARAM_LIST_NODE);
        add_to_list($$, $1);
    }
    | param_list COMMA param {
        add_to_list($1, $3);
        $$ = $1;
    }
    ;

param:
    type IDENTIFIER {
        $$ = create_ast_node(PARAM_NODE, 0, $2);
        add_child($$, $1);
    }
    ;

type:
    INTEGER {
        $$ = create_ast_node(INTEGER_TYPE_NODE, 0, NULL);
    }
    | BOOL {
        $$ = create_ast_node(BOOL_TYPE_NODE, 0, NULL);
    }
    ;

block:
    LBRACE var_decl_list statement_list RBRACE {
        $$ = create_ast_node(BLOCK_NODE, 0, NULL);
        if ($2) {
            for (int i = 0; i < $2->child_count; i++) {
                add_child($$, $2->children[i]);
            }
            free($2->children);
            free($2);
        }
        if ($3) {
            for (int i = 0; i < $3->child_count; i++) {
                add_child($$, $3->children[i]);
            }
            free($3->children);
            free($3);
        }
    }
    ;

statement_list:
    /* empty */ {
        $$ = create_list_node(STATEMENT_LIST_NODE);
    }
    | statement_list statement {
        add_to_list($1, $2);
        $$ = $1;
    }
    ;

statement:
    IDENTIFIER ASSIGN expr SEMICOLON {
        $$ = create_ast_node(ASSIGN_STMT_NODE, 0, NULL);
        add_child($$, create_ast_node(IDENTIFIER_NODE, 0, $1));
        add_child($$, $3);
    }
    | method_call SEMICOLON {
        $$ = create_ast_node(METHOD_CALL_STMT_NODE, 0, NULL);
        add_child($$, $1);
    }
    | IF LPAREN expr RPAREN THEN block ELSE block {
        $$ = create_ast_node(IF_STMT_NODE, 0, NULL);
        add_child($$, $3);
        add_child($$, $6);
        add_child($$, $8);
    }
    | WHILE expr block {
        $$ = create_ast_node(WHILE_STMT_NODE, 0, NULL);
        add_child($$, $2);
        add_child($$, $3);
    }
    | RETURN expr SEMICOLON {
        $$ = create_ast_node(RETURN_STMT_NODE, 0, NULL);
        add_child($$, $2);
    }
    | RETURN SEMICOLON {
        $$ = create_ast_node(RETURN_STMT_NODE, 0, NULL);
    }
    | block {
        $$ = $1;
    }
    ;

method_call:
    IDENTIFIER LPAREN arg_list RPAREN {
        $$ = create_ast_node(METHOD_CALL_NODE, 0, NULL);
        add_child($$, create_ast_node(IDENTIFIER_NODE, 0, $1));
        if ($3) add_child($$, $3);
    }
    ;

arg_list:
    /* empty */ {
        $$ = NULL;
    }
    | expr {
        $$ = create_list_node(ARG_LIST_NODE);
        add_to_list($$, $1);
    }
    | arg_list COMMA expr {
        add_to_list($1, $3);
        $$ = $1;
    }
    ;

expr:
    IDENTIFIER {
        $$ = create_ast_node(IDENTIFIER_NODE, 0, $1);
    }
    | method_call {
        $$ = $1;
    }
    | literal {
        $$ = $1;
    }
    | expr bin_op expr {
        $$ = create_ast_node(BINARY_OP_NODE, 0, NULL);
        add_child($$, $1);
        add_child($$, $2);
        add_child($$, $3);
    }
    | MINUS expr %prec UMINUS {
        $$ = create_ast_node(UNARY_OP_NODE, 0, NULL);
        add_child($$, create_ast_node(OPERATOR_NODE, 0, "-"));
        add_child($$, $2);
    }
    | NOT expr {
        $$ = create_ast_node(UNARY_OP_NODE, 0, NULL);
        add_child($$, create_ast_node(OPERATOR_NODE, 0, "!"));
        add_child($$, $2);
    }
    | LPAREN expr RPAREN {
        $$ = $2;
    }
    ;

bin_op:
    arith_op {
        $$ = $1;
    }
    | rel_op {
        $$ = $1;
    }
    | cond_op {
        $$ = $1;
    }
    ;

arith_op:
    PLUS {
        $$ = create_ast_node(ARITH_OP_NODE, 0, "+");
    }
    | MINUS {
        $$ = create_ast_node(ARITH_OP_NODE, 0, "-");
    }
    | MULTIPLY {
        $$ = create_ast_node(ARITH_OP_NODE, 0, "*");
    }
    | DIVIDE {
        $$ = create_ast_node(ARITH_OP_NODE, 0, "/");
    }
    | MODULO {
        $$ = create_ast_node(ARITH_OP_NODE, 0, "%");
    }
    ;

rel_op:
    LESS {
        $$ = create_ast_node(REL_OP_NODE, 0, "<");
    }
    | GREATER {
        $$ = create_ast_node(REL_OP_NODE, 0, ">");
    }
    | EQUAL {
        $$ = create_ast_node(REL_OP_NODE, 0, "==");
    }
    ;

cond_op:
    AND {
        $$ = create_ast_node(COND_OP_NODE, 0, "&&");
    }
    | OR {
        $$ = create_ast_node(COND_OP_NODE, 0, "||");
    }
    ;

literal:
    INTEGER_LITERAL {
        $$ = create_ast_node(INTEGER_LITERAL_NODE, $1, NULL);
    }
    | TRUE {
        $$ = create_ast_node(BOOL_LITERAL_NODE, 1, NULL);
    }
    | FALSE {
        $$ = create_ast_node(BOOL_LITERAL_NODE, 0, NULL);
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error sintáctico en línea %d: %s\n", yylineno, s);
}
