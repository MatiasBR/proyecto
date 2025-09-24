CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
FLEX = flex
BISON = bison
TARGET = c-tds

# Archivos fuente
LEX_FILE = src/lexer.l
YACC_FILE = src/parser.y
C_SOURCES = src/lexer.c src/parser.c src/ast.c src/symbol_table.c src/semantic.c src/intermediate.c src/main.c

# Archivos generados
LEX_C = src/lexer.c
YACC_C = src/parser.c
YACC_H = src/parser.h

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
src/lexer.o: src/lexer.c $(YACC_H)
src/parser.o: src/parser.c
src/main.o: src/main.c $(YACC_H)
src/ast.o: src/ast.c src/ast.h
src/symbol_table.o: src/symbol_table.c src/symbol_table.h
src/semantic.o: src/semantic.c src/semantic.h
src/intermediate.o: src/intermediate.c src/intermediate.h

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
