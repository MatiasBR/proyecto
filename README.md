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