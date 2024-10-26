# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread

# Targets and filenames
TARGETS = car controller call internal safety
TARGET_CAR = car
TARGET_CONTROLLER = controller
TARGET_CALL = call
TARGET_INTERNAL = internal
TARGET_SAFETY = safety

# Source files
SRC_CAR = car.c
SRC_CONTROLLER = controller.c
SRC_CALL = call.c
SRC_INTERNAL = internal.c
SRC_SAFETY = safety.c

# Header files
HEADERS = func.h

# Object files
OBJ_CAR = $(SRC_CAR:.c=.o)
OBJ_CONTROLLER = $(SRC_CONTROLLER:.c=.o)
OBJ_CALL = $(SRC_CALL:.c=.o)
OBJ_INTERNAL = $(SRC_INTERNAL:.c=.o)
OBJ_SAFETY = $(SRC_SAFETY:.c=.o)

# Default target to build all components
all: $(TARGETS)

# Rules for each component
car: $(OBJ_CAR)
	$(CC) $(CFLAGS) -o $(TARGET_CAR) $(OBJ_CAR)

controller: $(OBJ_CONTROLLER)
	$(CC) $(CFLAGS) -o $(TARGET_CONTROLLER) $(OBJ_CONTROLLER)

call: $(OBJ_CALL)
	$(CC) $(CFLAGS) -o $(TARGET_CALL) $(OBJ_CALL)

internal: $(OBJ_INTERNAL)
	$(CC) $(CFLAGS) -o $(TARGET_INTERNAL) $(OBJ_INTERNAL)

safety: $(OBJ_SAFETY)
	$(CC) $(CFLAGS) -o $(TARGET_SAFETY) $(OBJ_SAFETY)

# Clean target to remove object files and executables
clean:
	rm -f $(TARGETS) *.o

# Dependency rule for object files, ensuring each source file depends on func.h
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean car controller call internal safety
