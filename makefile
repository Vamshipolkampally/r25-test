CC = gcc
CFLAGS = -Og -g -Wall

# Source and include directories
SRC_DIR = src
LIB_DIR = lib
INCLUDE_DIR = include
TEST_DIR = test

# Executable name
EXE = main.exe

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(LIB_DIR)/*.c)

# Default target
all: build

# Build target
build: $(EXE)

$(EXE): $(SRC)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $(EXE) $(SRC) -lm

# Check target
check: $(EXE)
	@echo "--------------------------------------------"
	@echo "Checking..."
	@echo "Test-1: "
	./$(EXE) $(TEST_DIR)/sbus_data[1] $(TEST_DIR)/sab_data[1]
	diff $(TEST_DIR)/sab_data[1] $(TEST_DIR)/expected[1]
	@echo "Test-2: "
	./$(EXE) $(TEST_DIR)/sbus_data[2] $(TEST_DIR)/sab_data[2]
	diff $(TEST_DIR)/sab_data[2] $(TEST_DIR)/expected[2]
	@echo "Test-3: "
	./$(EXE) $(TEST_DIR)/sbus_data[3] $(TEST_DIR)/sab_data[3]
	diff $(TEST_DIR)/sab_data[3] $(TEST_DIR)/expected[3]
	@echo "**** Success: ***"
	@echo "--------------------------------------------"

# Clean target
clean:
	rm -f $(EXE)
	rm -f $(TEST_DIR)/sab_data[1]
	rm -f $(TEST_DIR)/sab_data[2]
	rm -f $(TEST_DIR)/sab_data[3]

# Phony targets
.PHONY: all build check clean
