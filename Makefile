CC = gcc

CFLAGS = -Wall -Wextra -pedantic

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
PUBLIC_DIR = public

TARGET = $(BIN_DIR)/server

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR) $(PUBLIC_DIR))

all: $(TARGET)

# Linking
$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $@

# Compiling each source file to object
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


production: CFLAGS += -O2
production: all


# Clean up build files
clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/*

.PHONY: all clean