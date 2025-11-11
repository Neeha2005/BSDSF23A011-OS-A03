# =====================================================
# Makefile for ROLL_NO-OS-A03 (Base Shell Project)
# =====================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude

# Directories
SRC_DIR = src
OBJ_DIR = build
BIN_DIR = bin

# Files
TARGET = $(BIN_DIR)/myshell
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/shell.c $(SRC_DIR)/execute.c
OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/shell.o $(OBJ_DIR)/execute.o

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)
	@echo "✅ Build complete: $(TARGET)"

# Compile .c → .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the shell
run: all
	@echo "🔹 Running myshell..."
	@$(TARGET)

# Clean compiled files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "🧹 Cleaned build directories."

# Phony targets (not actual files)
.PHONY: all clean run
