# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iheaders
CLINKS = -lncurses -lmenu

# Directories
SRC_DIR = src
HDR_DIR = headers
BIN_DIR = ../bin

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Output executable
TARGET = $(BIN_DIR)/treasure_hub

# Default rule
all: $(TARGET)

# Link all source files to create final executable
$(TARGET):
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(CLINKS)

# Clean rule
clean:
	rm -f $(TARGET)