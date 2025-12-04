#!/bin/bash

# ===============================================
#      COMPILADOR DE CALCULADORA DE EXPRESIONES
# ===============================================

echo "==============================================="
echo "   COMPILANDO CALCULADORA DE EXPRESIONES       "
echo "   SISTEMA: LINUX                              "
echo "==============================================="
echo ""

# Colores para mensajes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Función para mostrar mensajes
print_message() {
    echo -e "${GREEN}[+]${NC} $1"
}

print_error() {
    echo -e "${RED}[!] ERROR:${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[*]${NC} $1"
}

# Verificar que GCC está instalado
if ! command -v gcc &> /dev/null; then
    print_error "GCC no está instalado. Instálalo con:"
    echo "  Ubuntu/Debian: sudo apt install gcc"
    echo "  Fedora: sudo dnf install gcc"
    echo "  Arch: sudo pacman -S gcc"
    exit 1
fi

print_message "GCC encontrado: $(gcc --version | head -n1)"
echo ""

# Crear carpeta para ejecutables si no existe
if [ ! -d "bin" ]; then
    mkdir -p bin
    print_message "Carpeta 'bin' creada para ejecutables"
fi

# Limpiar compilaciones previas
print_message "Limpiando compilaciones previas..."
rm -f bin/*.o bin/*.out 2>/dev/null
rm -f *.o 2>/dev/null

# ===============================================
# COMPILAR ESTRUCTURAS DE DATOS
# ===============================================
print_message "Compilando estructuras de datos..."

gcc -c lib/list.c -Iinclude -Wall -Wextra -o list.o
if [ $? -ne 0 ]; then
    print_error "Error compilando list.c"
    exit 1
fi

gcc -c lib/dlist.c -Iinclude -Wall -Wextra -o dlist.o
if [ $? -ne 0 ]; then
    print_error "Error compilando dlist.c"
    exit 1
fi

gcc -c lib/stack.c -Iinclude -Wall -Wextra -o stack.o
if [ $? -ne 0 ]; then
    print_error "Error compilando stack.c"
    exit 1
fi

gcc -c lib/queue.c -Iinclude -Wall -Wextra -o queue.o
if [ $? -ne 0 ]; then
    print_error "Error compilando queue.c"
    exit 1
fi

print_message "Estructuras de datos compiladas exitosamente"
echo ""

# ===============================================
# COMPILAR MÓDULOS PRINCIPALES
# ===============================================
print_message "Compilando módulos principales..."

# 1. MainCalculator
print_warning "Compilando MainCalculator..."
gcc src/MainCalculator.c -o bin/MainCalculator -Wall -Wextra
if [ $? -ne 0 ]; then
    print_error "Error compilando MainCalculator"
    exit 1
fi

# 2. PRE-LETTERS
print_warning "Compilando PRE-LETTERS..."
gcc src/PRE-LETTERS.c list.o dlist.o stack.o -Iinclude -o bin/PRE-LETTERS -lm -Wall -Wextra
if [ $? -ne 0 ]; then
    print_error "Error compilando PRE-LETTERS"
    exit 1
fi

# 3. Infix
print_warning "Compilando Infix..."
gcc src/Infix.c list.o dlist.o stack.o queue.o -Iinclude -o bin/Infix -lm -Wall -Wextra
if [ $? -ne 0 ]; then
    print_error "Error compilando Infix"
    exit 1
fi

# 4. POSTFIX-LETTERS
print_warning "Compilando POSTFIX-LETTERS..."
gcc src/POSTFIX-LETTERS.c list.o dlist.o stack.o -Iinclude -o bin/POSTFIX-LETTERS -lm -Wall -Wextra
if [ $? -ne 0 ]; then
    print_error "Error compilando POSTFIX-LETTERS"
    exit 1
fi

# 5. PRE-NUM
print_warning "Compilando PRE-NUM..."
gcc src/PRE-NUM.c list.o dlist.o stack.o -Iinclude -o bin/PRE-NUM -lm -Wall -Wextra
if [ $? -ne 0 ]; then
    print_error "Error compilando PRE-NUM"
    exit 1
fi

# 6. POST-NUM
print_warning "Compilando POST-NUM..."
gcc src/POST-NUM.c list.o dlist.o stack.o -Iinclude -o bin/POST-NUM -lm -Wall -Wextra
if [ $? -ne 0 ]; then
    print_error "Error compilando POST-NUM"
    exit 1
fi

# Limpiar archivos objeto
rm -f *.o

# ===============================================
# CONFIGURAR PERMISOS DE EJECUCIÓN
# ===============================================
print_message "Configurando permisos de ejecución..."
chmod +x bin/*

# ===============================================
# VERIFICAR EJECUTABLES GENERADOS
# ===============================================
echo ""
print_message "==============================================="
print_message "      COMPILACIÓN COMPLETADA EXITOSAMENTE      "
print_message "==============================================="
echo ""
print_message "Ejecutables generados en la carpeta 'bin/':"
echo ""
ls -la bin/ | grep -v "^total"
echo ""
print_message "Tamaño de los ejecutables:"
du -h bin/* | while read size file; do
    filename=$(basename "$file")
    printf "  %-25s %s\n" "$filename" "$size"
done
echo ""

# ===============================================
# INSTRUCCIONES DE USO
# ===============================================
print_message "INSTRUCCIONES DE USO:"
echo ""
echo "  1. Para ejecutar el programa principal:"
echo "     ${BLUE}./bin/MainCalculator${NC}"
echo ""
echo "  2. Para ejecutar un módulo específico:"
echo "     ${BLUE}./bin/Infix${NC}        # Evaluador de expresiones infijas"
echo "     ${BLUE}./bin/PRE-LETTERS${NC}  # Prefijo con letras"
echo "     ${BLUE}./bin/POSTFIX-LETTERS${NC} # Postfijo con letras"
echo "     ${BLUE}./bin/PRE-NUM${NC}      # Prefijo con números"
echo "     ${BLUE}./bin/POST-NUM${NC}     # Postfijo con números"
echo ""
echo "  3. Para recompilar:"
echo "     ${BLUE}./compile.sh${NC}"
echo ""
print_message "¡Listo para usar! 🚀"
echo ""