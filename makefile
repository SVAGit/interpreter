SRC_DIR = source
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
TARGET = $(BIN_DIR)/program

CC = g++
CFLAGS = -std=c++23 -Wall -Wextra -g -I$(INC_DIR)

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "Linking $@..."
	@$(CC) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR) $(BIN_DIR):
	@mkdir -p $@

run: $(TARGET)
	@echo "Executing $<..."
	$<

clean:
	@echo "Deleting..."
	@rm -rf $(BIN_DIR) $(BUILD_DIR)

.PHONY: all clean
