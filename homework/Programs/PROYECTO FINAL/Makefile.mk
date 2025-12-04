# ===============================================
# MAKEFILE PARA CALCULADORA DE EXPRESIONES
# SISTEMA: LINUX
# ===============================================

# Compilador y banderas
CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c99
LDFLAGS = -lm
DEBUG_FLAGS = -g -DDEBUG
RELEASE_FLAGS = -O2

# Directorios
SRC_DIR = src
LIB_DIR = lib
INCLUDE_DIR = include
BIN_DIR = bin
OBJ_DIR = obj

# Archivos fuente
MAIN_SRC = $(SRC_DIR)/MainCalculator.c
MODULE_SRCS = $(wildcard $(SRC_DIR)/*.c)
MODULE_SRCS := $(filter-out $(MAIN_SRC), $(MODULE_SRCS))

# Archivos de librería
LIB_SRCS = $(wildcard $(LIB_DIR)/*.c)
LIB_OBJS = $(patsubst $(LIB_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIB_SRCS))

# Ejecutables
EXECUTABLES = $(BIN_DIR)/MainCalculator \
              $(BIN_DIR)/PRE-LETTERS \
              $(BIN_DIR)/Infix \
              $(BIN_DIR)/POSTFIX-LETTERS \
              $(BIN_DIR)/PRE-NUM \
              $(BIN_DIR)/POST-NUM

# Colores para mensajes
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
NC = \033[0m

# Mensajes
MSG_COMPILING = printf "${GREEN}[+]${NC} Compilando %s\n"
MSG_LINKING = printf "${BLUE}[*]${NC} Enlazando %s\n"
MSG_CLEANING = printf "${YELLOW}[*]${NC} Limpiando...\n"
MSG_DONE = printf "${GREEN}[+] COMPILACIÓN COMPLETADA${NC}\n"

# ===============================================
# REGLAS PRINCIPALES
# ===============================================

.PHONY: all debug release clean help run test

# Compilación por defecto (release)
all: release

# Compilación con información de depuración
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(EXECUTABLES)
	@$(MSG_DONE)

# Compilación optimizada
release: CFLAGS += $(RELEASE_FLAGS)
release: $(EXECUTABLES)
	@$(MSG_DONE)

# Crear directorios necesarios
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Compilar objetos de librerías
$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c | $(OBJ_DIR)
	@$(MSG_COMPILING) $<
	@$(CC) $(CFLAGS) -c $< -o $@

# MainCalculator (sin dependencias de librerías)
$(BIN_DIR)/MainCalculator: $(MAIN_SRC) | $(BIN_DIR)
	@$(MSG_COMPILING) $<
	@$(CC) $(CFLAGS) $< -o $@

# Módulos que dependen de list, dlist y stack
$(BIN_DIR)/PRE-LETTERS: $(SRC_DIR)/PRE-LETTERS.c $(LIB_OBJS) | $(BIN_DIR)
	@$(MSG_LINKING) $(notdir $@)
	@$(CC) $(CFLAGS) $< $(filter-out $(OBJ_DIR)/queue.o, $(LIB_OBJS)) -o $@ $(LDFLAGS)

$(BIN_DIR)/POSTFIX-LETTERS: $(SRC_DIR)/POSTFIX-LETTERS.c $(LIB_OBJS) | $(BIN_DIR)
	@$(MSG_LINKING) $(notdir $@)
	@$(CC) $(CFLAGS) $< $(filter-out $(OBJ_DIR)/queue.o, $(LIB_OBJS)) -o $@ $(LDFLAGS)

$(BIN_DIR)/PRE-NUM: $(SRC_DIR)/PRE-NUM.c $(LIB_OBJS) | $(BIN_DIR)
	@$(MSG_LINKING) $(notdir $@)
	@$(CC) $(CFLAGS) $< $(filter-out $(OBJ_DIR)/queue.o, $(LIB_OBJS)) -o $@ $(LDFLAGS)

$(BIN_DIR)/POST-NUM: $(SRC_DIR)/POST-NUM.c $(LIB_OBJS) | $(BIN_DIR)
	@$(MSG_LINKING) $(notdir $@)
	@$(CC) $(CFLAGS) $< $(filter-out $(OBJ_DIR)/queue.o, $(LIB_OBJS)) -o $@ $(LDFLAGS)

# Infix (depende de todas las librerías)
$(BIN_DIR)/Infix: $(SRC_DIR)/Infix.c $(LIB_OBJS) | $(BIN_DIR)
	@$(MSG_LINKING) $(notdir $@)
	@$(CC) $(CFLAGS) $< $(LIB_OBJS) -o $@ $(LDFLAGS)

# ===============================================
# REGLAS ADICIONALES
# ===============================================

# Ejecutar el programa principal
run: $(BIN_DIR)/MainCalculator
	@echo ""
	@printf "${GREEN}[+]${NC} Ejecutando MainCalculator...\n"
	@echo "==============================================="
	@./$(BIN_DIR)/MainCalculator

# Ejecutar todos los módulos en modo prueba
test: $(EXECUTABLES)
	@echo ""
	@printf "${GREEN}[+]${NC} Ejecutando pruebas...\n"
	@echo "==============================================="
	@for exe in $(EXECUTABLES); do \
		printf "${BLUE}[*]${NC} Probando $$(basename $$exe)... "; \
		if [ -x "$$exe" ]; then \
			printf "${GREEN}OK${NC}\n"; \
		else \
			printf "${RED}FALLÓ${NC}\n"; \
		fi \
	done

# Limpiar archivos generados
clean:
	@$(MSG_CLEANING)
	@rm -rf $(BIN_DIR) $(OBJ_DIR)
	@rm -f *.o core vgcore.*
	@printf "${GREEN}[+]${NC} Limpieza completada\n"

# Mostrar ayuda
help:
	@echo "==============================================="
	@echo "MAKEFILE PARA CALCULADORA DE EXPRESIONES"
	@echo "==============================================="
	@echo ""
	@echo "Comandos disponibles:"
	@echo "  ${GREEN}make${NC}           - Compilar en modo release (por defecto)"
	@echo "  ${GREEN}make debug${NC}     - Compilar con información de depuración"
	@echo "  ${GREEN}make release${NC}   - Compilar optimizado"
	@echo "  ${GREEN}make run${NC}       - Compilar y ejecutar MainCalculator"
	@echo "  ${GREEN}make test${NC}      - Verificar que todos los módulos se compilaron"
	@echo "  ${GREEN}make clean${NC}     - Eliminar archivos generados"
	@echo "  ${GREEN}make help${NC}      - Mostrar esta ayuda"
	@echo ""
	@echo "Ejecutables generados en: ${BLUE}bin/${NC}"
	@echo ""