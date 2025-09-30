# Compilador TDS25 - Estructura Modular

## Descripción
Compilador para el lenguaje TDS25 con arquitectura modular que incluye análisis léxico, sintáctico, semántico y generación de código intermedio.

## Estructura del Proyecto

```
proyecto/
├── src/
│   └── main.c                 # Punto de entrada principal
├── modules/
│   ├── ast/                  # Módulo de Árbol de Sintaxis Abstracta
│   │   ├── ast.h
│   │   └── ast.c
│   ├── lexer/                # Módulo de Análisis Léxico
│   │   └── lexer.l
│   ├── parser/               # Módulo de Análisis Sintáctico
│   │   └── parser.y
│   ├── semantic/             # Módulo de Análisis Semántico
│   │   ├── semantic.h
│   │   └── semantic.c
│   ├── intermediate/         # Módulo de Código Intermedio
│   │   ├── intermediate.h
│   │   └── intermediate.c
│   └── common/               # Utilidades Comunes
│       ├── symbol_table.h
│       └── symbol_table.c
├── Makefile                  # Configuración de compilación
└── test_*.ctds              # Archivos de prueba
```

## Módulos

### 1. AST (Abstract Syntax Tree)
- **Ubicación**: `modules/ast/`
- **Propósito**: Definición y manipulación del árbol de sintaxis abstracta
- **Archivos**: `ast.h`, `ast.c`

### 2. Lexer (Análisis Léxico)
- **Ubicación**: `modules/lexer/`
- **Propósito**: Tokenización del código fuente
- **Archivos**: `lexer.l` (especificación Flex)

### 3. Parser (Análisis Sintáctico)
- **Ubicación**: `modules/parser/`
- **Propósito**: Análisis sintáctico y construcción del AST
- **Archivos**: `parser.y` (especificación Bison)

### 4. Semantic (Análisis Semántico)
- **Ubicación**: `modules/semantic/`
- **Propósito**: Verificación semántica y tabla de símbolos
- **Archivos**: `semantic.h`, `semantic.c`

### 5. Intermediate (Código Intermedio)
- **Ubicación**: `modules/intermediate/`
- **Propósito**: Generación de código intermedio (IR)
- **Archivos**: `intermediate.h`, `intermediate.c`

### 6. Common (Utilidades)
- **Ubicación**: `modules/common/`
- **Propósito**: Funciones comunes como tabla de símbolos
- **Archivos**: `symbol_table.h`, `symbol_table.c`

## Compilación

```bash
# Compilar el proyecto
make

# Limpiar archivos generados
make clean

# Ejecutar pruebas
make test
```

## Dependencias

- **Flex**: Generador de analizadores léxicos
- **Bison**: Generador de analizadores sintácticos
- **GCC**: Compilador de C

## Instalación de Dependencias

```bash
# Ubuntu/Debian
sudo apt-get install flex bison gcc

# O usar el target del Makefile
make install-deps
```

## Uso

```bash
# Compilar un archivo .ctds
./c-tds archivo.ctds

# Con opciones
./c-tds -debug -target parse archivo.ctds
```

## Ventajas de la Estructura Modular

1. **Separación de responsabilidades**: Cada módulo tiene una función específica
2. **Mantenibilidad**: Fácil localización y modificación de código
3. **Reutilización**: Los módulos pueden ser reutilizados
4. **Escalabilidad**: Fácil agregar nuevas funcionalidades
5. **Organización**: Estructura clara y lógica

## Mejoras Implementadas

### Basadas en el Proyecto de Referencia

1. **AST Optimizado**: 
   - Uso de uniones para optimizar memoria
   - Estructuras específicas por tipo de nodo
   - Mejor manejo de listas con punteros `siguiente`

2. **Lexer Mejorado**:
   - Validación de rangos para enteros (int32)
   - Mejor manejo de errores léxicos
   - Soporte para enteros negativos

3. **Parser Robusto**:
   - Mensajes de error detallados (`parse.error verbose`)
   - Mejor manejo de conflictos

4. **Tabla de Símbolos Avanzada**:
   - Soporte para múltiples scopes
   - Validación semántica mejorada
   - Tipos de símbolos diferenciados

5. **Script de Compilación**:
   - Script `compile.sh` con opciones avanzadas
   - Modo debug integrado
   - Colores para mejor UX

## Scripts de Compilación

```bash
# Compilación básica
./compile.sh archivo.ctds

# Compilación con debug
./compile.sh archivo.ctds -debug

# Compilación con tests
./compile.sh archivo.ctds -test

# Limpiar y compilar
./compile.sh archivo.ctds -clean
```

## Testing del Análisis Semántico

### Comandos Básicos de Testing

```bash
# Compilar el proyecto
make

# Probar programa básico (funciona)
./c-tds -debug test.ctds

# Probar programa simple (funciona)
./c-tds -debug test_simple.ctds

# Ejecutar todos los tests
./test_runner.sh
```

### Archivos de Prueba Disponibles

- **`test.ctds`** - Programa básico con variable
- **`test_simple.ctds`** - Programa simple
- **`test_semantic_valid.ctds`** - Programa válido con funciones
- **`test_semantic_errors.ctds`** - Programa con errores semánticos
- **`test_semantic_no_main.ctds`** - Programa sin función main

### Opciones del Compilador

```bash
# Compilación básica
./c-tds archivo.ctds

# Con debug (recomendado para testing)
./c-tds -debug archivo.ctds

# Hasta análisis sintáctico
./c-tds -target parse archivo.ctds

# Hasta análisis semántico
./c-tds -target semantic archivo.ctds

# Hasta código intermedio
./c-tds -target codinter archivo.ctds

# Hasta assembly (por defecto)
./c-tds -target assembly archivo.ctds
```

### Reglas Semánticas Implementadas

El análisis semántico verifica las siguientes reglas del TDS25:

- ✅ **Regla 1**: No redeclaración en mismo scope
- ✅ **Regla 2**: Declaración antes de uso
- ✅ **Regla 3**: Función main obligatoria
- ✅ **Regla 4**: Argumentos de métodos correctos
- ✅ **Regla 5**: Métodos como expresión deben retornar valor
- ✅ **Regla 6**: Return void sin expresión
- ✅ **Regla 7**: Return con tipo correcto
- ✅ **Regla 8**: Variables declaradas
- ✅ **Regla 9**: Condiciones bool en if/while
- ✅ **Regla 10**: Operadores aritméticos con integer
- ✅ **Regla 11**: Operador == con mismo tipo
- ✅ **Regla 12**: Operadores lógicos con bool
- ✅ **Regla 13**: Asignaciones con tipos compatibles

### Ejemplos de Testing

```bash
# Test 1: Programa válido
./c-tds -debug test.ctds
# Resultado esperado: "Análisis semántico exitoso"

# Test 2: Programa simple
./c-tds -debug test_simple.ctds
# Resultado esperado: Compilación completa exitosa

# Test 3: Ver solo análisis semántico
./c-tds -target semantic test.ctds
# Resultado esperado: Solo análisis semántico sin generar código
```