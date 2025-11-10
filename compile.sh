#!/bin/bash

# Script de compilación mejorado para el compilador TDS25
# Basado en el proyecto de referencia pero adaptado a la estructura modular

set -e

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Función para imprimir mensajes con colores
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Verificar argumentos
if [ $# -eq 0 ]; then
    echo "Uso: $0 <archivo.ctds> [opciones]"
    echo "Opciones:"
    echo "  -debug     Compilar en modo debug"
    echo "  -clean     Limpiar archivos generados"
    echo "  -test      Ejecutar tests después de compilar"
    exit 1
fi

INPUT_FILE="$1"
DEBUG_MODE=0
CLEAN_MODE=0
TEST_MODE=0

# Parsear argumentos
shift
while [[ $# -gt 0 ]]; do
    case $1 in
        -debug)
            DEBUG_MODE=1
            shift
            ;;
        -clean)
            CLEAN_MODE=1
            shift
            ;;
        -test)
            TEST_MODE=1
            shift
            ;;
        *)
            print_error "Opción desconocida: $1"
            exit 1
            ;;
    esac
done

# Verificar que el archivo existe
if [ ! -f "$INPUT_FILE" ]; then
    print_error "El archivo '$INPUT_FILE' no existe."
    exit 1
fi

# Verificar extensión
if [[ ! "$INPUT_FILE" =~ \.ctds$ ]]; then
    print_error "El archivo debe tener extensión .ctds"
    exit 1
fi

print_info "Compilando archivo: $INPUT_FILE"

# Limpiar si se solicita
if [ $CLEAN_MODE -eq 1 ]; then
    print_info "Limpiando archivos generados..."
    make clean
fi

# Compilar el compilador
if [ $DEBUG_MODE -eq 1 ]; then
    print_info "Compilando en modo debug..."
    make clean
    make CFLAGS="$(make -n | grep CFLAGS | cut -d'=' -f2 | cut -d' ' -f2-)" DEBUG=1
else
    print_info "Compilando compilador..."
    make
fi

# Verificar que el compilador se generó correctamente
if [ ! -f "./c-tds" ]; then
    print_error "Error: No se pudo generar el compilador"
    exit 1
fi

print_info "Compilador generado exitosamente"

# Ejecutar el compilador con el archivo de entrada
print_info "Ejecutando compilador con $INPUT_FILE..."

if [ $DEBUG_MODE -eq 1 ]; then
    ./c-tds -debug "$INPUT_FILE"
else
    ./c-tds "$INPUT_FILE"
fi

# Verificar resultado
if [ $? -eq 0 ]; then
    print_info "Compilación exitosa"
    
    # Mostrar archivos generados
    BASE_NAME=$(basename "$INPUT_FILE" .ctds)
    print_info "Archivos generados:"
    for ext in sint sem ci ass out; do
        if [ -f "${BASE_NAME}.${ext}" ]; then
            echo "  - ${BASE_NAME}.${ext}"
        fi
    done
    
    # Ejecutar tests si se solicita
    if [ $TEST_MODE -eq 1 ]; then
        print_info "Ejecutando tests..."
        make test
    fi
else
    print_error "Error en la compilación"
    exit 1
fi

print_info "Proceso completado"
