CC = gcc
CFLAGS = -O0 -Wall -ggdb -I./include
LDFLAGS = -lm

# Bison flags
BFLAGS = -Wcounterexamples --debug

# Directories
BUILD_DIR = ./build

# Targets
TARGET = GLYPHIC

# Source files
BISON_FILE = GLYPHIC.y
FLEX_FILE = GLYPHIC.l
AST_FILE = ./include/ast.c
VARIABLE_STORAGE_FILE  = ./include/variable_storage.c
FUNCTION_STORAGE_FILE = ./include/function_storage.c
FUNCTION_PARAMS_FILE = ./include/function_params.c

# Generated files
BISON_C = $(BUILD_DIR)/GLYPHIC.tab.c
BISON_H = $(BUILD_DIR)/GLYPHIC.tab.h
LEX_C = $(BUILD_DIR)/lex.yy.c

BISON_O = $(BUILD_DIR)/GLYPHIC.tab.o
LEX_O = $(BUILD_DIR)/lex.yy.o
AST_O = $(BUILD_DIR)/ast.o
VARIABLE_STORAGE_O = $(BUILD_DIR)/variable_storage.o
FUNCTION_STORAGE_O = $(BUILD_DIR)/function_storage.o
FUNCTION_PARAMS_O = $(BUILD_DIR)/function_params.o

# Detect macOS and set BISON path accordingly
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
    BISON = /opt/homebrew/opt/bison/bin/bison
else
    BISON = bison
endif

# Default target
all: $(TARGET)

# Targets
$(TARGET): $(BISON_O) $(LEX_O) $(AST_O) $(VARIABLE_STORAGE_O) $(FUNCTION_STORAGE_O) $(FUNCTION_PARAMS_O)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BISON_C) $(BISON_H): $(BISON_FILE)
	$(BISON) --defines=$(BISON_H) $(BFLAGS) -o $(BISON_C) $(BISON_FILE)

$(BISON_O): $(BISON_C) $(BISON_H)
	$(CC) $(CFLAGS) -c $(BISON_C) -o $(BISON_O)

$(LEX_C): $(FLEX_FILE)
	flex -o $(LEX_C) $(FLEX_FILE)

$(LEX_O): $(LEX_C) $(BISON_H)
	$(CC) $(CFLAGS) -c $(LEX_C) -o $(LEX_O)

$(AST_O): $(AST_FILE)
	$(CC) $(CFLAGS) -c $(AST_FILE) -o $(AST_O)

$(VARIABLE_STORAGE_O): $(VARIABLE_STORAGE_FILE)
	$(CC) $(CFLAGS) -c $(VARIABLE_STORAGE_FILE) -o $(VARIABLE_STORAGE_O)

$(FUNCTION_STORAGE_O): $(FUNCTION_STORAGE_FILE)
	$(CC) $(CFLAGS) -c $(FUNCTION_STORAGE_FILE) -o $(FUNCTION_STORAGE_O)

$(FUNCTION_PARAMS_O): $(FUNCTION_PARAMS_FILE)
	$(CC) $(CFLAGS) -c $(FUNCTION_PARAMS_FILE) -o $(FUNCTION_PARAMS_O)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf ./graphs
	rm -rf $(TARGET)

# Ensure the build directory exists
$(shell mkdir -p $(BUILD_DIR))

.PHONY: all clean
