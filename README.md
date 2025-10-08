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
│   ├── assembly/             # Módulo de Código Assembly
│   │   ├── assembly.h
│   │   └── assembly.c
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

### 6. Assembly (Código Assembly)
- **Ubicación**: `modules/assembly/`
- **Propósito**: Generación de código assembly x86-64
- **Archivos**: `assembly.h`, `assembly.c`

### 7. Common (Utilidades)
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


### 📋 Instrucciones de Uso por Rama

```bash
make test
```

O ejecutar manualmente:

```bash
./test_runner.sh
```

## Archivos de Salida

El compilador genera diferentes archivos según la etapa:

- `.sint`: Salida del análisis sintáctico
- `.sem`: Salida del análisis semántico
- `.ci`: Código intermedio
- `.ass`: Código assembly
- `.out`: Ejecutable final

## Características del Lenguaje TDS25

### Tipos de Datos
- `integer`: Números enteros de 32 bits
- `bool`: Valores booleanos (true/false)
- `void`: Tipo de retorno para funciones sin valor

### Palabras Reservadas
- `program`, `bool`, `else`, `extern`, `false`, `if`, `integer`, `return`, `then`, `true`, `void`, `while`

### Operadores
- Aritméticos: `+`, `-`, `*`, `/`, `%`
- Relacionales: `<`, `>`, `==`
- Lógicos: `&&`, `||`, `!`
- Asignación: `=`

### Estructuras de Control
- `if-then-else`
- `while`
- `return`

## Implementación

### Análisis Léxico
- Implementado con flex
- Reconoce tokens, identificadores, literales y comentarios
- Maneja comentarios de línea (`//`) y multilínea (`/* */`)

### Análisis Sintáctico
- Implementado con bison
- Construye un AST (Abstract Syntax Tree)
- Verifica la estructura sintáctica del programa

### Análisis Semántico
- Verificación de tipos
- Gestión de alcance de variables
- Tabla de símbolos jerárquica
- Verificación de declaraciones antes de uso

### Código Intermedio
- Código de Tres Direcciones
- Generación de temporales
- Etiquetas para control de flujo
- Instrucciones para operaciones aritméticas y lógicas

## Limitaciones Actuales

1. **Generación de Assembly**: Implementación básica, no optimizada
2. **Optimizaciones**: No implementadas en esta versión
3. **Manejo de Errores**: Básico, mejoras pendientes
4. **Testing**: Cobertura limitada

## Branches de Entrega

### Branch Actual: `entrega-lexico-sintactico-ast`
**Fecha de entrega:** 24 de Septiembre 2025  
**Etapa:** Análisis Léxico y Sintáctico + Tabla de Símbolos + AST  

**Contenido:**
- ✅ Análizador Léxico (Scanner) con flex
- ✅ Análizador Sintáctico (Parser) con bison
- ✅ Tabla de Símbolos (TS) implementada
- ✅ Abstract Syntax Tree (AST) completo
- ✅ Interfaz CLI funcional
- ✅ Generación de archivos .sint
- ✅ Casos de test incluidos
- ✅ Documentación completa


Proyecto desarrollado para la materia Taller de Diseño de Software (Cod. 3306) - DC - FCEFQyN - UNRC
