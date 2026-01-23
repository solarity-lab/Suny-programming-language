CC = gcc

ifeq ($(OS),Windows_NT)
	PLATFORM = windows
else
	PLATFORM = linux
endif

SRCS = \
	MathLib.c \
	complex/LibComplex.c \
	complex/scomplex.c

CFLAGS = -Wall -Wextra -std=c11 -I. -Icomplex
LDFLAGS = -shared

ifeq ($(PLATFORM),windows)
	TARGET = mathlib.dll
	CLEAN  = del /Q
else
	TARGET = libmathlib.so
	CFLAGS += -fPIC
	CLEAN  = rm -f
endif

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) -o $(TARGET) -lSuny

clean:
	$(CLEAN) $(TARGET) 2>nul || true

.PHONY: all clean