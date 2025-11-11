CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lreadline

SRC = src/main.c src/shell.c src/execute.c src/builtins.c src/history.c src/tokenize_input.c
OBJ = $(SRC:src/%.c=build/%.o)
TARGET = bin/myshell

all: $(TARGET)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ) | bin
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

build:
	mkdir -p build

bin:
	mkdir -p bin

clean:
	rm -rf build bin
