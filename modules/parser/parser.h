/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER_LITERAL = 258,
     IDENTIFIER = 259,
     PROGRAM = 260,
     BOOL = 261,
     ELSE = 262,
     EXTERN = 263,
     FALSE = 264,
     IF = 265,
     INTEGER = 266,
     RETURN = 267,
     THEN = 268,
     TRUE = 269,
     VOID = 270,
     WHILE = 271,
     PLUS = 272,
     MINUS = 273,
     MULTIPLY = 274,
     DIVIDE = 275,
     MODULO = 276,
     LESS = 277,
     GREATER = 278,
     EQUAL = 279,
     AND = 280,
     OR = 281,
     NOT = 282,
     ASSIGN = 283,
     LPAREN = 284,
     RPAREN = 285,
     LBRACE = 286,
     RBRACE = 287,
     SEMICOLON = 288,
     COMMA = 289,
     ERROR = 290
   };
#endif
/* Tokens.  */
#define INTEGER_LITERAL 258
#define IDENTIFIER 259
#define PROGRAM 260
#define BOOL 261
#define ELSE 262
#define EXTERN 263
#define FALSE 264
#define IF 265
#define INTEGER 266
#define RETURN 267
#define THEN 268
#define TRUE 269
#define VOID 270
#define WHILE 271
#define PLUS 272
#define MINUS 273
#define MULTIPLY 274
#define DIVIDE 275
#define MODULO 276
#define LESS 277
#define GREATER 278
#define EQUAL 279
#define AND 280
#define OR 281
#define NOT 282
#define ASSIGN 283
#define LPAREN 284
#define RPAREN 285
#define LBRACE 286
#define RBRACE 287
#define SEMICOLON 288
#define COMMA 289
#define ERROR 290




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 18 "modules/parser/parser_simple_final.y"
{
    int int_val;
    char* string_val;
    ASTNode* node;
}
/* Line 1529 of yacc.c.  */
#line 125 "modules/parser/parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

