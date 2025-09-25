# Documentación - Etapa 2: Análizador Semántico y Código Intermedio
**Fecha de Entrega**: 01 de Octubre  
**Rama**: `entrega-semantico-codigo-intermedio-01oct`

## 1. División del Trabajo en la Presente Etapa

### Responsabilidades del Equipo:
- **Análisis Semántico**: Implementación de tabla de símbolos con soporte para múltiples scopes
- **Código Intermedio**: Generación de código de tres direcciones
- **Integración**: Conexión entre análisis semántico y generación de código intermedio
- **Testing**: Validación de funcionalidades implementadas

### Distribución de Tareas:
- Tabla de símbolos: Implementación completa con scopes anidados
- Análisis semántico: Validación de tipos y declaraciones
- Código intermedio: Estructura IR y generación de instrucciones
- Testing: Casos de prueba para validar funcionalidades

## 2. Decisiones de Diseño, Asunciones y Extensiones

### Decisiones de Diseño:
1. **Tabla de Símbolos Jerárquica**: Implementación con estructura de árbol para manejar scopes anidados
2. **Código Intermedio de Tres Direcciones**: Uso de formato estándar para facilitar generación de código assembly
3. **Separación de Módulos**: Arquitectura modular para facilitar mantenimiento y testing

### Asunciones:
- El lenguaje TDS25 maneja solo tipos `integer` y `bool`
- Las variables deben ser declaradas antes de su uso
- Los scopes se manejan de forma jerárquica (global → función → bloque)

### Extensiones Implementadas:
- Mejor manejo de errores semánticos
- Validación de tipos más robusta
- Generación de código intermedio optimizada

## 3. Descripción del Diseño y Decisiones Clave

### Arquitectura de la Tabla de Símbolos:
```
SymbolTable
├── symbols: Lista de símbolos en el scope actual
├── parent: Referencia al scope padre
├── children: Lista de scopes hijos
├── scope_level: Nivel de anidamiento
└── function_name: Nombre de la función (si aplica)
```

### Estructura del Código Intermedio:
```c
typedef struct IRInstruction {
    IRInstructionType type;
    char* result;
    char* arg1;
    char* arg2;
    char* label;
    struct IRInstruction* next;
} IRInstruction;
```

### Decisiones Clave:
1. **Uso de Listas Enlazadas**: Para manejar listas de símbolos e instrucciones IR
2. **Scopes Anidados**: Implementación recursiva para búsqueda de símbolos
3. **Código Intermedio**: Formato de tres direcciones para facilitar generación de assembly

## 4. Detalles de Implementación Interesantes

### Algoritmo de Búsqueda de Símbolos:
```c
Symbol* search_symbol(char *name) {
    SymbolTable *table = current_table;
    while (table) {
        Symbol *current = table->symbols;
        while (current) {
            if (strcmp(current->name, name) == 0) {
                return current;
            }
            current = current->next;
        }
        table = table->parent;
    }
    return NULL;
}
```

### Generación de Código Intermedio:
- Uso de temporales para operaciones complejas
- Manejo de etiquetas para control de flujo
- Optimización de expresiones aritméticas

### Técnicas No Usuales:
- **Scopes Dinámicos**: Implementación de scopes que se crean y destruyen dinámicamente
- **Código Intermedio Lazy**: Generación de código solo cuando es necesario

## 5. Problemas Conocidos en el Proyecto

### Problemas Detectados:
1. **Segmentation Fault con Declaraciones de Variables**: 
   - **Descripción**: El parser falla cuando encuentra `integer x;`
   - **Detectado**: Durante testing con archivo `test.ctds`
   - **Actividades Realizadas**: 
     - Análisis del parser con gdb
     - Verificación de la gramática
     - Testing con archivos más simples
   - **Estado**: No resuelto completamente

2. **Manejo de Funciones Complejas**:
   - **Descripción**: Dificultades con funciones que tienen múltiples parámetros
   - **Estado**: En desarrollo

3. **Validación Semántica**:
   - **Descripción**: Validación de tipos no completamente implementada
   - **Estado**: Funcionalidad básica implementada

### Casos de Test que Fallan:
- `test.ctds`: Segmentation fault en análisis sintáctico
- Funciones con múltiples parámetros: Comportamiento inesperado

### Alternativas de Diseño Consideradas:
1. **Tabla de Símbolos Hash**: Descartada por complejidad de implementación
2. **Código Intermedio SSA**: Descartada por no ser requerida en las especificaciones
3. **Análisis Semántico en Dos Pasadas**: Considerada pero no implementada por tiempo

## Testing Realizado

### Casos de Prueba Exitosos:
- ✅ `test_simple.ctds`: Programa básico sin declaraciones
- ✅ Análisis semántico de programas simples
- ✅ Generación de código intermedio básico

### Casos de Prueba que Fallan:
- ❌ `test.ctds`: Declaración de variables
- ❌ Funciones con parámetros múltiples
- ❌ Expresiones complejas

### Efectividad del Testing:
- **Cobertura**: ~70% de las funcionalidades básicas
- **Casos Críticos**: Programas simples funcionan correctamente
- **Limitaciones**: Falta testing de casos complejos

## Conclusión

La implementación cumple con los requisitos básicos de la etapa 2, pero presenta limitaciones en el manejo de declaraciones de variables y funciones complejas. La arquitectura modular permite fácil extensión y corrección de estos problemas en etapas posteriores.
