# Documentación - Optimizador

## Etapa: Optimizador/Extensiones

Implementación del módulo optimizador para el compilador TDS25.

## División del Trabajo

En esta etapa se implementó el módulo optimizador que aplica optimizaciones al código intermedio. El trabajo consistió en:

- Crear el módulo optimizer (optimizer.h y optimizer.c)
- Implementar 4 optimizaciones principales
- Integrar el optimizador en main.c para que funcione con la opción -opt
- Actualizar el Makefile
- Escribir esta documentación

## Decisiones de Diseño

### Arquitectura del Optimizador

Decidimos implementar el optimizador como un módulo separado en `modules/optimizer/` en lugar de agregarlo al módulo intermediate. Esto tiene sentido porque:

- El código intermedio ya estaba funcionando y no queríamos romperlo
- Es más fácil agregar nuevas optimizaciones sin tocar código existente
- Si hay un bug en el optimizador, no afecta al generador de IR
- Es más fácil de testear por separado

### Optimizaciones Implementadas

Se implementaron las siguientes optimizaciones estándar:

1. **Constant Folding (Plegado de Constantes)**
   - Evalúa expresiones constantes en tiempo de compilación
   - Reduce el número de operaciones en tiempo de ejecución
   - Aplica a operaciones aritméticas, lógicas y relacionales

2. **Dead Code Elimination (Eliminación de Código Muerto)**
   - Elimina código que nunca se ejecuta
   - Identifica etiquetas y bloques inalcanzables
   - Optimiza el tamaño del código generado

3. **Copy Propagation (Propagación de Copias)**
   - Reemplaza usos de variables con sus valores cuando es seguro
   - Elimina asignaciones redundantes (x = x)
   - Reduce dependencias entre variables temporales

4. **Common Subexpression Elimination (Eliminación de Subexpresiones Comunes)**
   - Detecta y reutiliza cálculos duplicados
   - Reduce operaciones redundantes
   - Mejora la eficiencia del código generado

### Orden de Aplicación

Probamos diferentes órdenes y este funcionó mejor:

1. Constant Folding primero - convierte expresiones como "5+3" en "8", lo que ayuda a las demás
2. Copy Propagation - después de constant folding hay más copias simples para propagar
3. Common Subexpression Elimination - copy propagation puede crear más subexpresiones duplicadas
4. Dead Code Elimination al final - elimina código que las otras optimizaciones volvieron innecesario

Si ponemos dead code elimination antes, puede eliminar código que luego las otras optimizaciones podrían usar.

### Integración con el Compilador

La opción `-opt` activa todas las optimizaciones. Cuando se usa:

- Si el target es `codinter` o `assembly`, se optimiza el IR antes de continuar
- Si es `codinter`, guarda el resultado en `.opt` en vez de `.ci` para distinguirlo
- Para `assembly`, usa el código optimizado para generar el assembly

Implementamos `-opt` sin parámetros porque según la consigna, si no se especifica optimización, se aplican todas. El parámetro opcional `[optimizacion]` lo dejamos para una futura extensión si se necesita.

## Detalles de Implementación

### Estructura de Datos

El optimizador utiliza las estructuras existentes del módulo `intermediate`:

```c
typedef struct IRInstruction {
    IRInstructionType type;
    char* result;
    char* arg1;
    char* arg2;
    char* label;
    struct IRInstruction* next;
} IRInstruction;

typedef struct {
    IRInstruction* instructions;
    int temp_count;
} IRCode;
```

### Algoritmos Implementados

#### Constant Folding

Recorre todas las instrucciones y si encuentra algo como `t1 = 5 + 3` donde ambos operandos son constantes, calcula el resultado (8) y lo reemplaza. Hace esto en un loop hasta que no haya más cambios.

La parte complicada fue manejar divisiones por cero - si encontramos `x / 0`, no optimizamos esa instrucción para evitar errores.

Ejemplo:
```
t1 = 5 + 3  ->  t1 = 8
t2 = 10 * 2 ->  t2 = 20
```

#### Dead Code Elimination

Esta fue la más difícil de implementar. El algoritmo:

1. Primero cuenta todas las etiquetas (labels) en el código
2. Marca cuáles son alcanzables empezando desde FUNC_START y siguiendo los saltos
3. Si una etiqueta nunca se alcanza, elimina todo el código hasta la siguiente etiqueta alcanzable

El problema es que hay que ser conservador - si eliminamos código que sí se ejecuta, el programa se rompe. Por eso el análisis es simple: solo seguimos GOTO e IF_GOTO explícitos. No intentamos determinar si una condición siempre es falsa (como `if (false)`), aunque eso sería útil.

Limitación: no elimina código después de `if (false)` porque no analizamos el valor de las condiciones, solo seguimos saltos.

#### Copy Propagation

Cuando vemos `x = y` o `x = 5`, buscamos todos los usos de `x` después de esa asignación y los reemplazamos con `y` o `5`. Nos detenemos si `x` se redefine o encontramos una etiqueta (porque podría haber un salto que cambie el flujo).

También eliminamos asignaciones inútiles como `x = x`.

Ejemplo:
```
t1 = 5
t2 = t1 + 3  ->  t2 = 5 + 3  (reemplazamos t1 por 5)
```

Esto ayuda porque después constant folding puede optimizar `5 + 3` a `8`.

#### Common Subexpression Elimination

Mantenemos una lista (caché) de expresiones que ya calculamos. Para cada operación binaria como `t2 = a + b`, generamos una clave única con el tipo de operación y los operandos. Si ya vimos `t1 = a + b` antes, en vez de calcular de nuevo, hacemos `t2 = t1`.

Limpiamos la caché cuando encontramos etiquetas o funciones porque no sabemos si los valores cambiaron.

Ejemplo:
```
t1 = a + b
t2 = a + b  ->  t2 = t1  (reutilizamos el resultado)
```

La clave se genera como "IR_ADD:a:b" para identificar la expresión.

### Funciones Auxiliares

- `is_constant(char* value)`: Verifica si un valor es una constante numérica
- `get_constant_value(char* value)`: Convierte una cadena constante a entero
- `int_to_string(int value)`: Convierte un entero a cadena
- `copy_ir_code(IRCode* ir_code)`: Crea una copia del código IR para optimización

### Gestión de Memoria

El optimizador:
- Crea copias del código IR para no modificar el original
- Libera memoria correctamente cuando reemplaza código optimizado
- Maneja correctamente las cadenas duplicadas en instrucciones IR

## Compatibilidad

El optimizador es compatible con:
- macOS (x86-64)
- Linux (x86-64)

No se requieren dependencias adicionales más allá de las del compilador base.

## Limitaciones Conocidas

### Dead Code Elimination

- El análisis de alcanzabilidad es conservador y puede no eliminar todo el código muerto
- No analiza profundamente condiciones complejas para determinar código inalcanzable

### Common Subexpression Elimination

- La caché se limpia en puntos de control, lo que puede ser demasiado agresivo
- No considera efectos secundarios de llamadas a funciones

### Copy Propagation

- No realiza análisis de alias avanzado
- Se detiene en puntos de control sin análisis más profundo

### Optimizaciones No Implementadas

Por limitaciones de tiempo, no se implementaron:
- Loop optimizations (unrolling, invariant code motion)
- Register allocation optimizations
- Strength reduction
- Function inlining

## Testing

### Casos de Prueba

Se probó el optimizador con:

1. **Programas con constantes**: Verificación de constant folding
2. **Programas con código muerto**: Verificación de eliminación
3. **Programas con copias**: Verificación de copy propagation
4. **Programas con subexpresiones comunes**: Verificación de CSE

### Uso

```bash
# Compilar con optimizaciones
./c-tds -opt archivo.ctds

# Compilar con optimizaciones y debug
./c-tds -opt -debug archivo.ctds

# Generar código intermedio optimizado
./c-tds -opt -target codinter archivo.ctds
```

## Problemas Conocidos

### Dead Code Elimination no elimina código después de if(false)

Cuando hay código como:
```
if (false) then {
    x = 5;
}
```

El dead code elimination no lo elimina porque no analizamos el valor de las condiciones, solo seguimos saltos. Para hacer esto bien habría que hacer análisis de flujo de datos más complejo, que no implementamos por tiempo.

**Cómo se detectó**: Probamos con un programa que tenía `if (false)` y vimos que el código seguía ahí en el .opt.

### Common Subexpression Elimination es muy conservador

Limpiamos la caché en cada etiqueta, lo que puede ser demasiado. Si tenemos:
```
L1: t1 = a + b
    goto L2
L2: t2 = a + b
```

No reutilizamos t1 aunque podríamos, porque limpiamos la caché en L2. Esto es conservador pero seguro.

**Cómo se detectó**: Al revisar el código optimizado notamos que no reutilizaba algunas subexpresiones que debería.

### Copy Propagation se detiene muy pronto

Nos detenemos en cualquier etiqueta, incluso si sabemos que el flujo siempre pasa por ahí. Esto es por seguridad pero podría ser más agresivo.

**Estado**: El código generado es correcto, solo podría estar más optimizado.

## Mejoras Futuras

Si tuviéramos más tiempo, implementaríamos:

1. Análisis de flujo de datos para detectar código muerto después de `if (false)`
2. Optimizaciones de bucles (loop unrolling, mover invariantes fuera del loop)
3. Análisis interprocedural para optimizar entre funciones
4. Permitir elegir optimizaciones específicas con `-opt constant_folding` por ejemplo

## Cambios Realizados en Etapas Anteriores

No hicimos cambios en etapas anteriores. El optimizador usa las estructuras IR que ya existían (IRCode, IRInstruction) sin modificarlas. Solo agregamos el módulo nuevo.

## Conclusión

El optimizador funciona y aplica las 4 optimizaciones principales. Hay cosas que se podrían mejorar pero cumple con lo pedido para esta etapa. El código optimizado es más eficiente que el no optimizado.

