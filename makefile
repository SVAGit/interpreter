CC = g++
CFLAGS = -std=c++23 -Wall -Wextra -g

SRC_DIR = source
INC_DIR = include

SRCS = $(wildcard $(SRC_DIR)/*.cpp)

TARGET = program

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -I$(INC_DIR) $^ -o $@