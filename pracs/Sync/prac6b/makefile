# Makefile for building the buffer program

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -pthread

# Source files
SRCS = skeleton_buffer.c

# Header files
HEADERS = buffer.h

# Executable name
EXEC = buffer_program

# Default target
all: $(EXEC)

# Rule to create the executable
$(EXEC): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRCS)

# Clean up the build
clean:
	rm -f $(EXEC)

.PHONY: all clean
