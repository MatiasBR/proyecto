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

## Estructura de Ramas del Proyecto

Este proyecto sigue el cronograma establecido en las especificaciones TDS25. Cada etapa tiene su propia rama:

### 🌿 Ramas por Etapa de Entrega

| Rama | Etapa | Fecha de Entrega | Estado |
|------|-------|------------------|--------|
| `main` | Rama principal estable | - | ✅ |
| `entrega-lexico-sintactico-ast` | **Etapa 1**: Análizador Léxico y Sintáctico | 15 de Septiembre | ✅ Completada |
| `entrega-parser-expandido-24sept` | **Etapa 1**: Parser Expandido | 24 de Septiembre | ✅ Completada |
| `entrega-semantico-codigo-intermedio-01oct` | **Etapa 2**: Análizador Semántico + Código Intermedio | 01 de Octubre | ✅ Completada |
| `entrega-codigo-objeto-27oct` | **Etapa 3**: Generador Código Objeto | 27 de Octubre | 🔄 En desarrollo |
| `entrega-optimizador-12nov` | **Etapa 4**: Optimizador | 12 de Noviembre | 📋 Pendiente |
| `entrega-final-15nov` | **Etapa 5**: Entrega Final | 15 de Noviembre | 📋 Pendiente |

### 📋 Instrucciones de Uso por Rama

```bash
# Cambiar a la rama de una etapa específica
git checkout entrega-semantico-codigo-intermedio-01oct

# Ver el estado actual
git status

# Compilar y probar
make clean && make
./c-tds -debug test.ctds
```

### 🎯 Estado Actual del Proyecto

- **✅ Etapa 1**: Análizador Léxico y Sintáctico - **COMPLETADA**
- **✅ Etapa 2**: Análizador Semántico y Código Intermedio - **COMPLETADA**  
- **🔄 Etapa 3**: Generador Código Objeto - **EN DESARROLLO**
- **📋 Etapa 4**: Optimizador - **PENDIENTE**
- **📋 Etapa 5**: Entrega Final - **PENDIENTE**

### 📊 Funcionalidades Implementadas

- ✅ **Análisis Léxico**: Flex con validación de enteros
- ✅ **Análisis Sintáctico**: Bison con gramática TDS25
- ✅ **Análisis Semántico**: Tabla de símbolos con scopes
- ✅ **Código Intermedio**: Código de tres direcciones
- ✅ **Código Assembly**: x86-64 assembly funcional
- ✅ **Interface CLI**: Todas las opciones requeridas
- ✅ **Archivos de Salida**: `.sint`, `.sem`, `.ci`, `.ass`, `.out`