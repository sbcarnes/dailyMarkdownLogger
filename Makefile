CC = gcc

# Libraries to include
LIBS = -lgdi32

# Prevent terminal window also opening
CFLAGS = -Wl,-subsystem,windows -mwindows

# Main source file
SRC = src/dailyLogger.c src/fileops.c

BUILD_DIR = build
BIN_DIR = bin

OBJ = $(SRC:src/%.c=$(BUILD_DIR)/%.o)

# Output executable
BIN = $(BIN_DIR)/dailyLogger.exe

# Default target
all: $(BIN)

# Link object files into the exe
$(BIN): $(OBJ)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CC) $(OBJ) -o $@ $(CFLAGS)

# Compile .c file into .o files
$(BUILD_DIR)/%.o: src/%.c
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean up build artifacts
clean:
	-if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	-if exist $(BIN_DIR) rmdir /s /q $(BIN_DIR)
