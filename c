# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lreadline

# Source and object files
SRC = src/main.c src/shell.c src/execute.c src/builtins.c
OBJ = $(SRC:src/%.c=build/%.o)

# Default target
all: bin/myshell_v4

# Build rules
build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

# Link final binaries for each feature version
bin/myshell_v1: $(OBJ) | bin
	$(CC) $(OBJ) -o $@

bin/myshell_v2: $(OBJ) | bin
	$(CC) $(OBJ) -o $@

bin/myshell_v3: $(OBJ) | bin
	$(CC) $(OBJ) -o $@

bin/myshell_v4: $(OBJ) | bin
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Create directories if they don't exist
build:
	mkdir -p build

bin:
	mkdir -p bin

# Cleanup
clean:
	rm -rf build bin
