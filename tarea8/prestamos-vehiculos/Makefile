CC ?= gcc
PKG_CONFIG ?= pkg-config

CFLAGS ?= -std=c11 -O2 -g -Wall -Wextra -Wno-deprecated-declarations
GTK_CFLAGS := $(shell $(PKG_CONFIG) --cflags gtk+-3.0)
GTK_LIBS := $(shell $(PKG_CONFIG) --libs gtk+-3.0)

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

TARGET := build/autocredito$(EXE)

.PHONY: all run ejemplo test clean

all: $(TARGET)

build:
	mkdir -p build

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(GTK_LIBS) -lm

run: $(TARGET)
	./$(TARGET)

ejemplo: $(TARGET)
	./$(TARGET) --data data/ejemplo.json

test: $(TARGET)
	./$(TARGET) --selftest

clean:
	rm -rf build
