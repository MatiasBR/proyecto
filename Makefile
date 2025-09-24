CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
FLEX = flex
BISON = bison
TARGET = c-tds

# Archivos fuente
LEX_FILE = modules/lexer/lexer.l
YACC_FILE = modules/parser/parser.y
C_SOURCES = modules/lexer/lexer.c modules/parser/parser.c modules/ast/ast.c modules/common/symbol_table.c modules/semantic/semantic.c modules/intermediate/intermediate.c modules/assembly/assembly.c src/main.c

# Archivos generados
LEX_C = modules/lexer/lexer.c
YACC_C = modules/parser/parser.c
YACC_H = modules/parser/parser.h

# Objetos
OBJECTS = $(C_SOURCES:.c=.o)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) -lfl

# Generar lexer
$(LEX_C): $(LEX_FILE)
	$(FLEX) -o $(LEX_C) $(LEX_FILE)

# Generar parser
$(YACC_C) $(YACC_H): $(YACC_FILE)
	$(BISON) -d -o $(YACC_C) $(YACC_FILE)

# Compilar objetos
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dependencias
modules/lexer/lexer.o: modules/lexer/lexer.c $(YACC_H)
modules/parser/parser.o: modules/parser/parser.c
src/main.o: src/main.c $(YACC_H)
modules/ast/ast.o: modules/ast/ast.c modules/ast/ast.h
modules/common/symbol_table.o: modules/common/symbol_table.c modules/common/symbol_table.h
modules/semantic/semantic.o: modules/semantic/semantic.c modules/semantic/semantic.h
modules/intermediate/intermediate.o: modules/intermediate/intermediate.c modules/intermediate/intermediate.h
modules/assembly/assembly.o: modules/assembly/assembly.c modules/assembly/assembly.h

clean:
	rm -f $(TARGET) $(OBJECTS) $(LEX_C) $(YACC_C) $(YACC_H)
	rm -f *.lex *.sint *.sem *.ci *.ass *.opt *.out

test: $(TARGET)
	@echo "Running tests..."
	@./test_runner.sh

install-deps:
	@echo "Installing dependencies..."
	@sudo apt-get update
	@sudo apt-get install -y flex bison gcc

.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all        - Build the compiler"
	@echo "  clean      - Remove generated files"
	@echo "  test       - Run tests"
	@echo "  install-deps - Install system dependencies"
