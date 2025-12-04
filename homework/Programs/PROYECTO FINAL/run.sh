#!/bin/bash

# ===============================================
#     EJECUTOR DE CALCULADORA DE EXPRESIONES
# ===============================================

# Colores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Verificar si está compilado
check_compiled() {
    if [ ! -d "bin" ] || [ ! -f "bin/MainCalculator" ]; then
        echo -e "${YELLOW}[*]${NC} El programa no está compilado."
        read -p "¿Deseas compilarlo ahora? (s/n): " choice
        
        if [ "$choice" = "s" ] || [ "$choice" = "S" ]; then
            if [ -f "compile.sh" ]; then
                chmod +x compile.sh
                ./compile.sh
            elif [ -f "Makefile" ]; then
                make
            else
                echo -e "${RED}[!]${NC} No se encontró script de compilación."
                exit 1
            fi
        else
            echo -e "${RED}[!]${NC} El programa necesita ser compilado primero."
            exit 1
        fi
    fi
}

# Menú principal
show_menu() {
    clear
    echo -e "${GREEN}===============================================${NC}"
    echo -e "${GREEN}      CALCULADORA DE EXPRESIONES - LINUX       ${NC}"
    echo -e "${GREEN}===============================================${NC}"
    echo ""
    echo "Selecciona una opción:"
    echo ""
    echo "  ${BLUE}1)${NC} Menú principal (MainCalculator)"
    echo "  ${BLUE}2)${NC} Evaluador Infix"
    echo "  ${BLUE}3)${NC} Prefijo con letras"
    echo "  ${BLUE}4)${NC} Postfijo con letras"
    echo "  ${BLUE}5)${NC} Prefijo con números"
    echo "  ${BLUE}6)${NC} Postfijo con números"
    echo "  ${BLUE}7)${NC} Probar todos los módulos"
    echo "  ${BLUE}8)${NC} Compilar"
    echo "  ${BLUE}9)${NC} Limpiar compilaciones"
    echo "  ${BLUE}0)${NC} Salir"
    echo ""
}

# Función para ejecutar un módulo
run_module() {
    module=$1
    name=$2
    
    if [ -f "bin/$module" ]; then
        echo -e "${GREEN}[+]${NC} Ejecutando $name..."
        echo "==============================================="
        ./bin/$module
        echo ""
        echo -e "${GREEN}[+]${NC} $name finalizado"
        echo "Presiona Enter para continuar..."
        read
    else
        echo -e "${RED}[!]${NC} $name no está compilado."
        read -p "¿Compilar ahora? (s/n): " choice
        if [ "$choice" = "s" ] || [ "$choice" = "S" ]; then
            ./compile.sh
        fi
    fi
}

# Función para probar todos los módulos
test_all() {
    echo -e "${GREEN}[+]${NC} Probando todos los módulos..."
    echo ""
    
    modules=("MainCalculator" "PRE-LETTERS" "Infix" "POSTFIX-LETTERS" "PRE-NUM" "POST-NUM")
    names=("Menú Principal" "Prefijo Letras" "Evaluador Infix" "Postfijo Letras" "Prefijo Números" "Postfijo Números")
    
    for i in "${!modules[@]}"; do
        if [ -f "bin/${modules[$i]}" ]; then
            echo -e "  ${GREEN}✓${NC} ${names[$i]} - ${BLUE}OK${NC}"
        else
            echo -e "  ${RED}✗${NC} ${names[$i]} - ${RED}FALTANTE${NC}"
        fi
    done
    
    echo ""
    echo -e "${GREEN}[+]${NC} Prueba completada"
    echo "Presiona Enter para continuar..."
    read
}

# Programa principal
main() {
    check_compiled
    
    while true; do
        show_menu
        read -p "Opción: " option
        
        case $option in
            1)
                run_module "MainCalculator" "Menú Principal"
                ;;
            2)
                run_module "Infix" "Evaluador Infix"
                ;;
            3)
                run_module "PRE-LETTERS" "Prefijo con Letras"
                ;;
            4)
                run_module "POSTFIX-LETTERS" "Postfijo con Letras"
                ;;
            5)
                run_module "PRE-NUM" "Prefijo con Números"
                ;;
            6)
                run_module "POST-NUM" "Postfijo con Números"
                ;;
            7)
                test_all
                ;;
            8)
                if [ -f "compile.sh" ]; then
                    ./compile.sh
                elif [ -f "Makefile" ]; then
                    make
                else
                    echo -e "${RED}[!]${NC} No se encontró script de compilación."
                fi
                echo "Presiona Enter para continuar..."
                read
                ;;
            9)
                echo -e "${YELLOW}[*]${NC} Limpiando compilaciones..."
                rm -rf bin obj *.o 2>/dev/null
                echo -e "${GREEN}[+]${NC} Limpieza completada"
                echo "Presiona Enter para continuar..."
                read
                ;;
            0)
                echo -e "${GREEN}[+]${NC} Saliendo..."
                exit 0
                ;;
            *)
                echo -e "${RED}[!]${NC} Opción inválida"
                echo "Presiona Enter para continuar..."
                read
                ;;
        esac
    done
}

# Ejecutar programa principal
main