# Define the compiler
CC = gcc

# Define the compiler flags
CFLAGS = -Wall -Wextra -std=c11

# Define the target executable
TARGET = shm_ipc

# Define the source files
SRCS = shm_ipc.c

# Define the object files
OBJS = $(SRCS:.c=.o)

# Define the header files
HEADERS = operations.h

# Default target
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile source files into object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(TARGET)

# PHONY targets are not actual files, just commands to be executed
.PHONY: all clean
