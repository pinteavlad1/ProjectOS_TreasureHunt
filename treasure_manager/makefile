# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iheaders

# Directories
SRC_DIR = src
HDR_DIR = headers
BIN_DIR = ../bin

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Output executable
TARGET = $(BIN_DIR)/treasure_manager

# Default rule
all: $(TARGET)

# Link all source files to create final executable
$(TARGET):
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

# Clean rule
clean:
	rm -f $(TARGET)
	rm -rf hunts
	rm -f logged_hunt-*