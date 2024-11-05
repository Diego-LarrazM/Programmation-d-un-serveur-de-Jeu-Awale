LIBS  = 
CFLAGS = -Wall
CC = gcc

SRCS = $(wildcard Serveur/*.c) $(wildcard Client/*.c)
TARGETS = $(SRCS:.c=)


.PHONY: $(TARGETS) start_server
all: $(TARGETS)

$(TARGETS): $(SRCS)
	$(CC) $< $(LIBS) $(CFLAGS) -o $@

start_server:
	./Serveur/server

start_client:
	./Client/client
	