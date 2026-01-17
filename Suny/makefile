CC = gcc
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
OUT = suny
DEBUG = dsuny
EXPORT = -rdynamic
LIB = libSuny.a
OPT = osuny
OPT_FLAGS = -O3 -flto -march=native -fomit-frame-pointer -fno-asynchronous-unwind-tables -DNDEBUG

ifeq ($(OS),Windows_NT)
    RM = del /Q
    EXT = .exe
else
    RM = rm -f
    EXT =
endif

all: $(OUT)$(EXT) $(DEBUG)$(EXT) $(OPT)$(EXT) $(LIB)

superdebug:
	@echo Building super debug exe...
	@$(CC) -g -O0 -fsanitize=address -fno-omit-frame-pointer $(SRC) -o sdsuny

bin: $(OUT)$(EXT)

debug: $(DEBUG)$(EXT)

lib: $(LIB)

optimize: $(OPT)$(EXT)


$(OUT)$(EXT): $(SRC)
	@echo Building release exe...
	@$(CC) -g $(EXPORT) $(SRC) -o $@

$(DEBUG)$(EXT): $(SRC)
	@echo Building debug exe...
	@$(CC) -g -O0 $(EXPORT) -Wall -Wextra -DDEBUG $(SRC) -o $@

$(LIB): $(OBJ)
	@echo Building static library...
	ar rcs $(LIB) $(OBJ)

$(OPT)$(EXT): $(SRC)
	@echo Building OPTIMIZED exe...
	@$(CC) $(OPT_FLAGS) $(EXPORT) $(SRC) -o $@

%.o: %.c Suny.h
	$(CC) -c $< -o $@

clean:
	$(RM) *.o
