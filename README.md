# Compilador TDS25

Compilador para el lenguaje TDS25 implementado como proyecto para la materia Taller de Diseño de Software (Cod. 3306) de la Universidad Nacional de Río Cuarto.

## Descripción

Este compilador implementa las siguientes etapas:

1. **Análisis Léxico y Sintáctico**: Scanner y Parser usando flex/bison
2. **Análisis Semántico**: Verificación de tipos y alcance de variables
3. **Generación de Código Intermedio**: Código de Tres Direcciones
4. **Generación de Código Assembly**: Código assembly x86-64 básico

## Estructura del Proyecto

```
proyecto/
├── Makefile              # Archivo de construcción
├── src/
│   ├── lexer.l           # Especificación del lexer (flex)
│   ├── parser.y          # Especificación del parser (bison)
│   ├── ast.h/c           # Estructura del AST
│   ├── symbol_table.h/c   # Tabla de símbolos
│   ├── semantic.h/c       # Análisis semántico
│   ├── intermediate.h/c   # Generación de código intermedio
│   └── main.c            # Programa principal
├── test_runner.sh        # Script de pruebas
└── README.md             # Este archivo
```

## Requisitos

- flex (lexer generator)
- bison (parser generator)
- gcc (compiler)
- make

### Instalación de dependencias (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install flex bison gcc make
```

## Compilación

```bash
make
```

Esto generará el ejecutable `c-tds`.

## Uso

```bash
./c-tds [opcion] nombreArchivo.ctds
```

### Opciones

- `-o <salida>`: Renombra el archivo ejecutable
- `-target <etapa>`: Compila hasta la etapa especificada (scan, parse, codinter, assembly)
- `-opt [optimizacion]`: Realiza optimizaciones
- `-debug`: Imprime información de debugging
- `-h, --help`: Muestra la ayuda

### Ejemplos

```bash
# Compilar hasta análisis sintáctico
./c-tds -target parse programa.ctds

# Compilar hasta código intermedio
./c-tds -target codinter programa.ctds

# Compilar completamente con debug
./c-tds -debug programa.ctds
```

## Ejecutar Tests

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

## Desarrollo Futuro

- [ ] Implementar optimizaciones
- [ ] Mejorar generación de assembly
- [ ] Ampliar suite de tests
- [ ] Implementar más verificaciones semánticas
- [ ] Mejorar manejo de errores

## Branches de Entrega

### Branch Actual: `entrega-lexico-sintactico-ast`
**Fecha de entrega:** 24 de Septiembre 2025  
**Etapa:** Análisis Léxico y Sintáctico + Tabla de Símbolos + AST  
**Estado:** ✅ COMPLETADO

**Contenido:**
- ✅ Análizador Léxico (Scanner) con flex
- ✅ Análizador Sintáctico (Parser) con bison
- ✅ Tabla de Símbolos (TS) implementada
- ✅ Abstract Syntax Tree (AST) completo
- ✅ Interfaz CLI funcional
- ✅ Generación de archivos .sint
- ✅ Casos de test incluidos
- ✅ Documentación completa

### Próximas Entregas:
- **Branch:** `entrega-semantico` (1 de Octubre 2025)
- **Branch:** `entrega-codigo-intermedio` (8 de Octubre 2025)
- **Branch:** `entrega-codigo-objeto` (27 de Octubre 2025)
- **Branch:** `entrega-optimizador` (12 de Noviembre 2025)
- **Branch:** `entrega-final` (15 de Noviembre 2025)

## Autores

Proyecto desarrollado para la materia Taller de Diseño de Software (Cod. 3306) - DC - FCEFQyN - UNRC

## Licencia

Este proyecto es parte de un trabajo académico.
