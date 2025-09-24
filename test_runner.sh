#!/bin/bash

# Script para ejecutar tests del compilador TDS25

echo "=== Test Runner para Compilador TDS25 ==="

# Verificar que el compilador existe
if [ ! -f "./c-tds" ]; then
    echo "Error: El compilador 'c-tds' no existe. Ejecute 'make' primero."
    exit 1
fi

# Crear directorio de tests si no existe
mkdir -p tests

# Test 1: Programa básico
echo "Test 1: Programa básico con main"
cat > tests/test1.ctds << 'EOF'
program {
    void main() {
        integer x = 5;
        if (x > 0) then {
            return;
        }
        else {
            return;
        }
    }
}
EOF

echo "Ejecutando test1.ctds..."
./c-tds -debug tests/test1.ctds
echo ""

# Test 2: Programa con funciones
echo "Test 2: Programa con funciones"
cat > tests/test2.ctds << 'EOF'
program {
    integer add(integer a, integer b) {
        return a + b;
    }
    
    void main() {
        integer result = add(3, 4);
        return;
    }
}
EOF

echo "Ejecutando test2.ctds..."
./c-tds -debug tests/test2.ctds
echo ""

# Test 3: Programa con while
echo "Test 3: Programa con while"
cat > tests/test3.ctds << 'EOF'
program {
    void main() {
        integer i = 0;
        while (i < 10) {
            i = i + 1;
        }
        return;
    }
}
EOF

echo "Ejecutando test3.ctds..."
./c-tds -debug tests/test3.ctds
echo ""

# Test 4: Programa con funciones externas
echo "Test 4: Programa con funciones externas"
cat > tests/test4.ctds << 'EOF'
program {
    integer get_int() extern;
    void print_int(integer i) extern;
    
    void main() {
        integer x = get_int();
        print_int(x);
        return;
    }
}
EOF

echo "Ejecutando test4.ctds..."
./c-tds -debug tests/test4.ctds
echo ""

# Test 5: Programa con operaciones aritméticas
echo "Test 5: Programa con operaciones aritméticas"
cat > tests/test5.ctds << 'EOF'
program {
    integer calculate(integer a, integer b) {
        integer sum = a + b;
        integer diff = a - b;
        integer prod = a * b;
        return sum + diff + prod;
    }
    
    void main() {
        integer result = calculate(10, 5);
        return;
    }
}
EOF

echo "Ejecutando test5.ctds..."
./c-tds -debug tests/test5.ctds
echo ""

# Test 6: Programa con operaciones lógicas
echo "Test 6: Programa con operaciones lógicas"
cat > tests/test6.ctds << 'EOF'
program {
    bool is_positive(integer x) {
        return x > 0;
    }
    
    bool is_even(integer x) {
        return (x % 2) == 0;
    }
    
    void main() {
        integer x = 10;
        bool pos = is_positive(x);
        bool even = is_even(x);
        return;
    }
}
EOF

echo "Ejecutando test6.ctds..."
./c-tds -debug tests/test6.ctds
echo ""

# Test 7: Programa con errores sintácticos (debe fallar)
echo "Test 7: Programa con errores sintácticos (debe fallar)"
cat > tests/test7.ctds << 'EOF'
program {
    void main() {
        integer x = 5
        if (x > 0) then {
            return;
        }
    }
}
EOF

echo "Ejecutando test7.ctds (debe fallar)..."
./c-tds -debug tests/test7.ctds
echo ""

# Test 8: Programa con errores semánticos (debe fallar)
echo "Test 8: Programa con errores semánticos (debe fallar)"
cat > tests/test8.ctds << 'EOF'
program {
    void main() {
        integer x = true;
        bool y = 5;
        return;
    }
}
EOF

echo "Ejecutando test8.ctds (debe fallar)..."
./c-tds -debug tests/test8.ctds
echo ""

echo "=== Tests completados ==="
echo "Revisar los archivos de salida generados (.sint, .sem, .ci, .ass)"
