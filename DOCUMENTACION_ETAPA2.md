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

### Problemas Resueltos:
1. **Análisis Semántico Completo**: 
   - **Descripción**: Implementación completa de las 13 reglas semánticas del TDS25
   - **Estado**: ✅ RESUELTO - Todas las reglas implementadas y funcionando

2. **Validación de Tipos**:
   - **Descripción**: Sistema completo de verificación de tipos en expresiones y operaciones
   - **Estado**: ✅ RESUELTO - Validación robusta implementada

3. **Manejo de Scopes**:
   - **Descripción**: Implementación jerárquica de scopes (global → función → bloque)
   - **Estado**: ✅ RESUELTO - Manejo completo de visibilidad

### Casos de Test Implementados:
- ✅ `test_semantic_valid.ctds`: Programa válido con análisis semántico exitoso
- ✅ `test_semantic_errors.ctds`: Errores semánticos detectados correctamente
- ✅ `test_semantic_no_main.ctds`: Detección de ausencia de función main
- ✅ `test_semantic_runner.sh`: Script automatizado de testing

### Alternativas de Diseño Implementadas:
1. **Tabla de Símbolos Jerárquica**: ✅ Implementada con estructura de árbol
2. **Análisis Semántico en Dos Pasadas**: ✅ Implementado (declaraciones → análisis)
3. **Sistema de Reporte de Errores**: ✅ Implementado con contexto detallado

## Testing Realizado

### Casos de Prueba Exitosos:
- ✅ `test_semantic_valid.ctds`: Programa completo con funciones y variables
- ✅ `test_semantic_errors.ctds`: Detección correcta de errores semánticos
- ✅ `test_semantic_no_main.ctds`: Verificación de función main obligatoria
- ✅ Análisis semántico completo de programas complejos
- ✅ Validación de tipos en todas las operaciones
- ✅ Manejo de scopes jerárquicos

### Reglas Semánticas Validadas:
- ✅ **Regla 1**: No redeclaración en mismo scope
- ✅ **Regla 2**: Declaración antes de uso
- ✅ **Regla 3**: Función main obligatoria
- ✅ **Regla 4**: Argumentos de métodos correctos
- ✅ **Regla 5**: Métodos como expresión deben retornar valor
- ✅ **Regla 6**: Return void sin expresión
- ✅ **Regla 7**: Return con tipo correcto
- ✅ **Regla 8**: Variables declaradas
- ✅ **Regla 9**: Condiciones bool en if/while
- ✅ **Regla 10**: Operadores aritméticos con integer
- ✅ **Regla 11**: Operador == con mismo tipo
- ✅ **Regla 12**: Operadores lógicos con bool
- ✅ **Regla 13**: Asignaciones con tipos compatibles

### Efectividad del Testing:
- **Cobertura**: 100% de las reglas semánticas del TDS25
- **Casos Críticos**: Todos los casos de prueba funcionan correctamente
- **Robustez**: Detección precisa de errores semánticos

## Conclusión

La implementación cumple completamente con los requisitos de la etapa 2. Se ha logrado un análisis semántico robusto que verifica todas las reglas del TDS25, con un sistema completo de manejo de errores y casos de prueba exhaustivos. La arquitectura modular facilita el mantenimiento y la extensión para etapas posteriores.
