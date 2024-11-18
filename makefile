LIBS  = 
CFLAGS = 
CC = gcc

# Source files
SRCS_S = $(wildcard Serveur/*.c) 
SRCS_A = $(wildcard Libraries/Awale/*.c)
SRCS_C = $(wildcard Client/*.c)

# Object files
OBJS_A = $(SRCS_A:.c=.o)
OBJS_S = $(SRCS_S:.c=.o)
OBJS_C = $(SRCS_C:.c=.o)

# Targets
TARGETS_C = $(SRCS_C:.c=)
TARGETS_S = $(SRCS_S:.c=)

.PHONY: all clean

# Default target
all: $(TARGETS_C) $(TARGETS_S)

# Build each target in Client
$(TARGETS_C): $(OBJS_C)
	$(CC) $< $(LIBS) $(CFLAGS) -o $@

# Build each target in Serveur with dependency on Libraries/Awale objects
$(TARGETS_S): $(OBJS_S) $(OBJS_A)
	$(CC) $(OBJS_A) $< $(LIBS) $(CFLAGS) -o $@

# Build object files for Libraries/Awale
%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

# Clean up
clean:
	rm -f Serveur/*.o Libraries/Awale/*.o Client/*.o $(TARGETS_S) $(TARGETS_C)
