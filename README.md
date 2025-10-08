# Compilador TDS25 - Estructura Modular

## Descripción
Compilador para el lenguaje TDS25 con arquitectura modular que incluye análisis léxico, sintáctico, semántico y generación de código intermedio. 

## Estado del Proyecto (8 de Octubre 2025)
- ✅ **Análisis Léxico y Sintáctico**: Funcionando correctamente
- ✅ **Análisis Semántico**: Implementado y funcionando
- ✅ **Generador de Código Intermedio**: Código de tres direcciones implementado
- ✅ **Front-end del compilador**: Completado

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
# Compilar un archivo .ctds hasta código intermedio
./c-tds -debug -target codinter archivo.ctds

# Análisis léxico y sintáctico
./c-tds -debug -target parse archivo.ctds

# Análisis semántico
./c-tds -debug -target sem archivo.ctds

# Ver ayuda
./c-tds --help
```

## Ejemplos de Uso

```bash
# Crear un programa de prueba
echo "program { }" > test.ctds

# Compilar hasta código intermedio
./c-tds -debug -target codinter test.ctds

# Salida esperada:
# === Código Intermedio ===
# t1 = 0
```


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

- **`test.ctds`** - Programa básico (funciona)
- **`test_simple.ctds`** - Programa simple (funciona)
- **`test_empty.ctds`** - Programa vacío (funciona)
- **`test_final.ctds`** - Programa de prueba final (funciona)
- **`test1.ctds` a `test5.ctds`** - Programas de prueba múltiple (funcionan)

### Estado de las Pruebas (8 de Octubre)
- ✅ **Programas vacíos**:
- ✅ **Análisis léxico**:
- ✅ **Análisis sintáctico**: 
- ✅ **Análisis semántico**: 
- ✅ **Generación IR**: 

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

### Generador de Código Intermedio

**Estado**: ✅ **COMPLETADO PARA ENTREGA DEL 8 DE OCTUBRE**

- ✅ **Código de Tres Direcciones**: Implementado
- ✅ **Estructura IR**: Completa para operaciones aritméticas, lógicas y control de flujo
- ✅ **Generación básica**: Funcionando correctamente
- ✅ **Pipeline completo**: Desde análisis hasta código intermedio


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


