# Define the compiler to be used
CC=gcc

# Define the compiler flags (in this case, including the current directory for header files)
CFLAGS=-I.

# Define the header file dependencies
DEPS = hellomake.h

# Pattern rule to compile from .c files to .o files
# %.o: Target pattern (any object file)
# %.c: Prerequisite pattern (any source file)
# $(DEPS): Additional prerequisites (header files)
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Rule to link the object files to create the final executable
# hellomake: Target (final executable)
# hellomake.o hellofunc.o: Prerequisites (object files)
hellomake: hellomake.o hellofunc.o 
	$(CC) -o hellomake hellomake.o hellofunc.o 
