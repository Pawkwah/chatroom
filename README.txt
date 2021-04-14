CS 3251 PA 1 - Chatroom

Parker Harris
pharris36@gatech.edu

Files:
	chatclient.c -> A client driver for chatserver.c
	chatserver.c -> A server setup for a chatroom, accessed through chatclient.c
	Makefile -> compilation tool

Intructions to run the program.

1. Go to root directory of the project
2. execute "make"
3. to start server -> ./bin/chatserver -p <PORT NUMBER>
4. to Start clients -> ./bin/chatclient -p <PORT NUMBER -u <USERNAME -s <PASSWORD>

Dependencies:
	Makefile
	Gcc 
	(Runs with build-essential package)
	ran on ubunutu 18.04

Known limitations:
	The server can only receive up to 1024 clients in its lifetime before a buffer overflow will occur.
	The client cannot handle a ^C stop during execution. 
	A server shutdown while clients still running causes infinite loop. 

Design choices:
	Currently, a message over 1024 characters will continue to send messages until the buffer is empty and all the characters are sent.
	The emoticons will be reflected to the user that sent them, but any other message will only be reflected on the other clients. 

