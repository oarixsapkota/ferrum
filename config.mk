C_GCC := gcc
C_STD := -std=c99
C_LIB := 
C_FLG := $(C_STD) $(C_LIB) -Wall -Wextra
C_SRC := $(wildcard src/*.c) $(wildcard src/*.h) $(wildcard src/**/*.c) $(wildcard src/**/*.h)
PROJ := ferrum
