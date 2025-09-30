#!/bin/bash

echo "=== Test Runner para Análisis Semántico ==="

# Verificar que el compilador existe
if [ ! -f "./c-tds" ]; then
    echo "Error: El compilador 'c-tds' no existe. Ejecute 'make' primero."
    exit 1
fi

echo "Test 1: Programa válido"
echo "Ejecutando test_semantic_valid.ctds..."
./c-tds -debug test_semantic_valid.ctds
echo ""

echo "Test 2: Programa con errores semánticos"
echo "Ejecutando test_semantic_errors.ctds..."
./c-tds -debug test_semantic_errors.ctds
echo ""

echo "Test 3: Programa sin función main"
echo "Ejecutando test_semantic_no_main.ctds..."
./c-tds -debug test_semantic_no_main.ctds
echo ""

echo "=== Tests completados ==="
