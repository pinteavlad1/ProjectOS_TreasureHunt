# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iheaders

# Directories
SRC_DIR = src
OBJ_DIR = objects
HDR_DIR = headers

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Output executable
TARGET = treasure_manager

# Default rule
all: $(TARGET)

# Link all object files to create final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c files to .o in objects/ directory
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure objects/ directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
