# Compilador TDS25

Compilador para el lenguaje TDS25 hecho para Taller de Diseño de Software (3306) de la UNRC.

## Estructura

El proyecto está dividido en módulos, uno por cada etapa del compilador:

- `modules/lexer/` - Scanner (flex)
- `modules/parser/` - Parser (bison) 
- `modules/ast/` - AST
- `modules/common/` - Tabla de símbolos
- `modules/semantic/` - Análisis semántico
- `modules/intermediate/` - Código intermedio
- `modules/assembly/` - Assembly x86-64
- `modules/optimizer/` - Optimizador
- `src/` - Main

## Compilar

```bash
make
```

Genera el ejecutable `c-tds`. Para limpiar:

```bash
make clean
```

## Uso

```bash
./c-tds [opciones] archivo.ctds
```

Opciones:
- `-o <salida>` - Nombre del ejecutable
- `-target <etapa>` - Hasta qué etapa compilar (scan, parse, codinter, assembly)
- `-opt [optimizacion]` - Aplicar optimizaciones
- `-debug` - Mostrar info de debug
- `-h, --help` - Ayuda

Ejemplos:
```bash
./c-tds -target codinter programa.ctds
./c-tds -opt all programa.ctds
./c-tds -target assembly programa.ctds
```

## Ramas

El proyecto se hizo por etapas, cada una en su rama:

### entrega-lexico-sintactico-ast
Primera entrega. Scanner con flex, parser con bison, AST básico.

### entrega-parser-expandido-24sept
Parser expandido con más construcciones.

### entrega-semantico-codigo-intermedio-01oct
Análisis semántico y código intermedio. Tabla de símbolos, verificación de tipos, código de tres direcciones.

### entrega-codigo-objeto-27oct
Generación de assembly x86-64. Manejo de registros y stack.

### entrega-optimizador-12nov
Optimizador con constant folding, dead code elimination, copy propagation y CSE.

### entrega-final-15nov
Entrega final con todo integrado. Interfaz completa, documentación y tests.

### main
Rama principal con la versión estable.

## Tests

Hay varios archivos de test en la raíz: `test_simple.ctds`, `test_func.ctds`, `test_if.ctds`, `test_opt.ctds`, `test_completo.ctds` y otros.

## Documentación

La documentación está en `DOCUMENTACION_FINAL.md`. Los PDFs de especificación están en `proyec.txt` y `proyec2.txt`.

## Requisitos

Necesitás flex, bison, gcc, as y ld.

Ubuntu/Debian:
```bash
sudo apt-get install flex bison gcc binutils
```

macOS:
```bash
brew install flex bison
```
