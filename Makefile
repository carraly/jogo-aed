# Compilador
CC = gcc

# Flags de compilação (Avisos, Otimização, Includes e rastreamento de dependências)
CFLAGS = -Wall -std=c99 -O2 -I include -MMD -MP

# Detecção do Sistema Operacional
ifeq ($(OS),Windows_NT)
    # Configurações para Windows
    LDFLAGS = -L lib/windows -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET = jogo-aed.exe
    RM = del /Q
    CLEAN_FILES = $(subst /,\,$(OBJ) $(DEP) $(TARGET))
else
    # Configurações para Linux
    LDFLAGS = -L lib/linux -lraylib -lm -lpthread -ldl -lrt
    TARGET = jogo-aed
    RM = rm -f
    CLEAN_FILES = $(OBJ) $(DEP) $(TARGET)
endif

# Encontra todos os arquivos .c na pasta src/
SRC = $(wildcard src/*.c)

# Transforma a lista de .c em .o (objetos) e .d (dependências)
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

# Regra padrão
all: $(TARGET)

# Regra para gerar o executável final
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Regra para compilar cada arquivo .c em um .o separadamente
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para compilar e já rodar o jogo
run: $(TARGET)
	./$(TARGET)

# Regra para limpar os arquivos compilados
clean:
	-$(RM) $(CLEAN_FILES)

# Declara as regras que não são arquivos físicos
.PHONY: all run clean

# Inclui os arquivos de dependência gerados pelo GCC (-MMD -MP)
-include $(DEP)
