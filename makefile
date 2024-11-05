LIBS  = 
CFLAGS = 
CC = gcc

SRCS_S = $(wildcard Serveur/*.c) 
SRCS_C = $(wildcard Client/*.c)
TARGETS_C = $(SRCS_C:.c=)
TARGETS_S = $(SRCS_S:.c=)


.PHONY: $(TARGETS_C) $(TARGETS_S)
all: $(TARGETS_C) $(TARGETS_S)

$(TARGETS_C): $(SRCS_C)
	$(CC) $< $(LIBS) $(CFLAGS) -o $@

$(TARGETS_S): $(SRCS_S)
	$(CC) $< $(LIBS) $(CFLAGS) -o $@

