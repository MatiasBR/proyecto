# Compilador TDS25

Compilador completo para el lenguaje TDS25 desarrollado para la materia Taller de Diseño de Software (Código 3306) de la Universidad Nacional de Río Cuarto.

## Estructura del Proyecto

El proyecto está organizado en módulos que corresponden a cada etapa del compilador:

- `modules/lexer/` - Analizador léxico (flex)
- `modules/parser/` - Analizador sintáctico (bison)
- `modules/ast/` - Árbol sintáctico abstracto
- `modules/common/` - Tabla de símbolos y utilidades compartidas
- `modules/semantic/` - Analizador semántico
- `modules/intermediate/` - Generador de código intermedio
- `modules/assembly/` - Generador de código assembly x86-64
- `modules/optimizer/` - Optimizador de código intermedio
- `src/` - Punto de entrada del compilador

## Compilación

Para compilar el proyecto:

```bash
make
```

Esto generará el ejecutable `c-tds`. Para limpiar archivos generados:

```bash
make clean
```

## Uso

El compilador se ejecuta con:

```bash
./c-tds [opciones] archivo.ctds
```

Opciones disponibles:
- `-o <salida>` - Especifica el nombre del ejecutable de salida
- `-target <etapa>` - Compila hasta la etapa especificada (scan, parse, codinter, assembly)
- `-opt [optimizacion]` - Aplica optimizaciones al código intermedio
- `-debug` - Muestra información de debugging durante la compilación
- `-h, --help` - Muestra la ayuda

Ejemplos:
```bash
# Compilar hasta código intermedio
./c-tds -target codinter programa.ctds

# Compilar con optimizaciones
./c-tds -opt all programa.ctds

# Compilar hasta assembly y generar ejecutable
./c-tds -target assembly programa.ctds
```

## Ramas del Repositorio

El proyecto se desarrolló de forma incremental, con una rama por cada entrega:

### entrega-lexico-sintactico-ast
Primera entrega con el analizador léxico y sintáctico. Incluye:
- Implementación del scanner con flex
- Parser con bison
- Construcción del AST básico
- Validación sintáctica

### entrega-parser-expandido-24sept
Expansión del parser para soportar más construcciones del lenguaje.

### entrega-semantico-codigo-intermedio-01oct
Segunda entrega con análisis semántico y generación de código intermedio:
- Tabla de símbolos completa
- Verificación de tipos
- Validación de reglas semánticas
- Generación de código de tres direcciones

### entrega-codigo-objeto-27oct
Tercera entrega con generador de código objeto:
- Generación de código assembly x86-64
- Manejo de registros y stack
- Compilación a ejecutables

### entrega-optimizador-12nov
Cuarta entrega con el módulo optimizador:
- Constant folding
- Dead code elimination
- Copy propagation
- Common subexpression elimination
- Integración con la opción -opt

### entrega-final-15nov
Rama principal de la entrega final que integra todas las etapas:
- Todas las funcionalidades anteriores
- Interfaz de línea de comandos completa
- Documentación final
- Casos de test completos

### main
Rama principal del repositorio. Contiene la versión más estable del compilador.

## Archivos de Test

El proyecto incluye varios archivos de test en la raíz:
- `test_simple.ctds` - Programa básico de prueba
- `test_func.ctds` - Pruebas de funciones
- `test_if.ctds` - Pruebas de estructuras condicionales
- `test_opt.ctds` - Pruebas específicas del optimizador
- `test_completo.ctds` - Programa más complejo
- Y otros archivos de prueba

## Documentación

La documentación detallada del optimizador se encuentra en `DOCUMENTACION_OPTIMIZADOR.md`.

Los documentos de especificación del proyecto están en:
- `proyec.txt` - Especificación del lenguaje TDS25
- `proyec2.txt` - Descripción del proyecto y requisitos

## Requisitos

- flex (o lex)
- bison (o yacc)
- gcc
- as (ensamblador)
- ld (linker)

En sistemas basados en Debian/Ubuntu:
```bash
sudo apt-get install flex bison gcc binutils
```

En macOS:
```bash
brew install flex bison
```

## Autores

Proyecto desarrollado para Taller de Diseño de Software - UNRC 2025
