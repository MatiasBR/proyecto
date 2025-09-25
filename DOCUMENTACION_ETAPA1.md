# Documentación - Etapa 1: Análizador Léxico y Sintáctico
**Fecha de Entrega**: 15 de Septiembre  
**Rama**: `entrega-lexico-sintactico-ast`

## 1. División del Trabajo en la Presente Etapa

### Responsabilidades del Equipo:
- **Análisis Léxico**: Implementación del scanner con Flex
- **Análisis Sintáctico**: Implementación del parser con Bison
- **Árbol de Sintaxis Abstracta**: Estructura de datos para representar el programa
- **Integración**: Conexión entre lexer, parser y AST

### Distribución de Tareas:
- Lexer: Implementación de tokens y manejo de errores léxicos
- Parser: Gramática TDS25 y construcción del AST
- AST: Estructura de datos y funciones de manipulación
- Testing: Validación de análisis léxico y sintáctico

## 2. Decisiones de Diseño, Asunciones y Extensiones

### Decisiones de Diseño:
1. **Estructura AST con Arrays**: Uso de arrays de hijos para facilitar acceso
2. **Manejo de Errores Robusto**: Implementación de funciones de reporte de errores
3. **Validación de Enteros**: Verificación de rangos para literales enteros

### Asunciones:
- El lenguaje TDS25 es case-sensitive
- Los enteros son de 32 bits con signo
- Los comentarios pueden ser de línea o multilínea

### Extensiones Implementadas:
- Validación de rangos para enteros (int32)
- Mejor manejo de errores léxicos
- Soporte para enteros negativos
- Mensajes de error detallados

## 3. Descripción del Diseño y Decisiones Clave

### Arquitectura del Lexer:
```c
// Validación de enteros con verificación de rangos
[-]?[0-9]+ {
    errno = 0;
    long long val = strtol(yytext, NULL, 10);
    if (errno == ERANGE || val < INT32_MIN || val > INT32_MAX) {
        reportar_error("Literal entero fuera de rango", yytext, yylineno);
    } else {
        yylval.int_val = (int)val;
        return INTEGER_LITERAL;
    }
}
```

### Estructura del AST:
```c
typedef struct ASTNode {
    ASTNodeType type;
    int int_value;
    char* string_value;
    struct ASTNode** children;
    int child_count;
} ASTNode;
```

### Decisiones Clave:
1. **Uso de Flex y Bison**: Herramientas estándar para análisis léxico y sintáctico
2. **AST con Arrays**: Estructura simple pero eficiente para representar el programa
3. **Manejo de Errores**: Funciones centralizadas para reporte de errores

## 4. Detalles de Implementación Interesantes

### Algoritmo de Construcción del AST:
```c
ASTNode* create_ast_node(ASTNodeType type, int int_value, char* string_value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type;
    node->int_value = int_value;
    node->string_value = string_value ? strdup(string_value) : NULL;
    node->children = NULL;
    node->child_count = 0;
    return node;
}
```

### Manejo de Errores Léxicos:
```c
void reportar_error(const char *msg, const char *text, int lineno) {
    fprintf(stderr, "Error léxico en línea %d: %s: '%s'\n", lineno, msg, text);
}
```

### Técnicas No Usuales:
- **Validación de Rangos**: Verificación de overflow en literales enteros
- **Manejo de Memoria**: Uso de `strdup` para strings y liberación automática
- **Parser con Verbose**: Habilitación de mensajes de error detallados

## 5. Problemas Conocidos en el Proyecto

### Problemas Detectados:
1. **Segmentation Fault con Declaraciones**:
   - **Descripción**: El parser falla con `integer x;`
   - **Detectado**: Durante testing con archivos de prueba
   - **Actividades Realizadas**: 
     - Análisis de la gramática
     - Verificación de conflictos shift/reduce
     - Testing con archivos más simples
   - **Estado**: No resuelto completamente

2. **Conflictos Shift/Reduce**:
   - **Descripción**: 2 conflictos detectados por Bison
   - **Estado**: Documentados pero no resueltos

3. **Manejo de Funciones**:
   - **Descripción**: Dificultades con funciones complejas
   - **Estado**: Funcionalidad básica implementada

### Casos de Test que Fallan:
- ❌ `test.ctds`: Segmentation fault
- ❌ Funciones con parámetros múltiples
- ❌ Expresiones complejas

### Alternativas de Diseño Consideradas:
1. **AST con Uniones**: Descartada por complejidad de implementación
2. **Parser Recursivo Manual**: Descartada por preferir Bison
3. **Manejo de Errores con Excepciones**: No aplicable en C

## Testing Realizado

### Casos de Prueba Exitosos:
- ✅ `test_simple.ctds`: Programa básico sin declaraciones
- ✅ Análisis léxico de tokens básicos
- ✅ Construcción de AST para programas simples

### Casos de Prueba que Fallan:
- ❌ `test.ctds`: Declaración de variables
- ❌ Funciones con parámetros
- ❌ Expresiones complejas

### Efectividad del Testing:
- **Cobertura**: ~60% de las funcionalidades básicas
- **Casos Críticos**: Tokens y gramática básica funcionan
- **Limitaciones**: Falta testing de casos complejos

## Conclusión

La implementación cumple con los requisitos básicos de la etapa 1, pero presenta limitaciones en el manejo de declaraciones de variables. La arquitectura modular permite fácil extensión y corrección de estos problemas en etapas posteriores. El lexer y parser básicos funcionan correctamente para programas simples.
