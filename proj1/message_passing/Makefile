# This file shows a simple Makefile. 
BIN = CLIENT SERVER
LIB = -lm #use library /usr/lib/libm for unresolved references
CFLAGS = -g #use -g flag to produce modules for debugger
CC = gcc #name of the compiler
#
all: $(BIN)

CLIENT:		client.o
		$(CC) -o $@ client.o $(LIB)

SERVER:		server.o
		$(CC) -o $@ server.o $(LIB)

client.o:	client.c mymsg.h
server.o:	server.c mymsg.h

clean:		
		rm -f *.o
