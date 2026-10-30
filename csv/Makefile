CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS :=
LDLIBS  := -lm

SRC_DIR := src
OBJ_DIR := build


ifeq ($(OS),Windows_NT)
    BIN := run.exe
  
    MKDIR_P := mkdir -p
    RM_RF   := rm -rf
    
    ifeq ($(shell where rm 2>NUL),)
        MKDIR_P := if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
        RM_RF   := if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)" & if exist "$(BIN)" del /q "$(BIN)"
    endif
else
    BIN := run
    MKDIR_P := mkdir -p
    RM_RF   := rm -rf
endif

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all clean rebuild

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	$(MKDIR_P) $(OBJ_DIR)

clean:
	$(RM_RF) $(OBJ_DIR) $(BIN)

rebuild: clean all