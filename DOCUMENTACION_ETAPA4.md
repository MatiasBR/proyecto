# Documentación Técnica

## Implementación

El generador de código assembly se implementó en el módulo `assembly.c`. La implementación incluye:

- Generación de código assembly x86-64
- Gestión de variables en stack frame
- Soporte para operaciones aritméticas y lógicas
- Compatibilidad con macOS y Linux

## Decisiones de Diseño

### Arquitectura

- Variables almacenadas en stack frame con offsets de 8 bytes
- Registros rax, rbx, rcx, rdx para operaciones temporales
- Convención de llamadas x86-64 estándar

### Compatibilidad

- Soporte para macOS y Linux
- Detección automática de syscalls
- Sintaxis assembly compatible con ambos sistemas

## Estructura del Código

### AssemblyContext

```c
typedef struct {
    Variable* variables;
    int var_count;
    int temp_count;
    int label_count;
    int current_offset;
} AssemblyContext;
```

### Gestión de Variables

Stack frame con offsets incrementales

Registro de variables y temporales

Gestión automática de memoria

## Algoritmos

### Generación de Assembly

- Mapeo de instrucciones IR a código assembly
- Switch-case para diferentes tipos de operaciones
- Generación de código x86-64

### Gestión de Variables

- Registro de variables en contexto
- Cálculo automático de offsets
- Gestión de temporales

## Limitaciones

### Parser

- Solo acepta programas vacíos (`program { }`)
- No maneja declaraciones de variables complejas
- Limitado para pruebas avanzadas

### Optimizaciones

- No se implementaron optimizaciones de código
- Código assembly básico sin optimizaciones
- Enfoque en corrección sobre eficiencia

## Testing

### Casos de Prueba

- Programas básicos que funcionan con el parser actual
- Verificación de todas las opciones de línea de comandos
- Testing de generación de assembly

### Resultados

- Compilación exitosa hasta assembly x86-64
- Generación de ejecutables funcionales
- Archivos de salida correctos (.ass, .out, .ci, .sem, .sint)

## Archivos Generados

- `.ass` - Código assembly x86-64
- `.out` - Ejecutable compilado
- `.ci` - Código intermedio
- `.sem` - Análisis semántico
- `.sint` - Análisis sintáctico

## Uso

```bash
# Compilación básica
./c-tds archivo.ctds

# Con debug
./c-tds -debug archivo.ctds

# Hasta etapa específica
./c-tds -target assembly archivo.ctds
```
