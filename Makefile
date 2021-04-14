CC = gcc
CFLAGS = -g
LDFLAGS =
LIBS = .
OBJS = chatclient chatserver

all: $(OBJS)

chatclient:
	$(CC) -o bin/chatclient src/chatclient.c -lrt -pthread

chatserver: 
	$(CC) -o bin/chatserver src/chatserver.c -lrt -pthread 

clean:
	@rm bin/chatclient bin/chatserver
	@echo Cleaned!