#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

static pthread_mutex_t thread = PTHREAD_MUTEX_INITIALIZER;

int port = 5001; 
char username[1024];
char password[1024]; 
void* receiver(void* client)
{
	int clientSocket = *((int*) client);
	while(1)
	{
		int len; 
		char* buffer; 
		len = recv(clientSocket, buffer, 1055, 0);
		buffer[len] = '\0';
		printf("%s\n", buffer);
	}
}	

int parse_args(int argc, char *argv[])
{
	int opt;
	while ((opt = getopt(argc, argv, "p:u:s:")) != -1) {
        switch (opt) {
        case 'p':
            port = atoi(optarg);
            printf("Port number: %d\n", port);
            break;
        case 'u':
        	strcpy(username, optarg);
        	printf("Username: %s\n", username);
        	break;
        case 's':
        	strcpy(password, optarg);
        	printf("Password: %s\n", password);
        	break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-p port] [-u username] [-s password] \n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}
int main(int argc, char *argv[])
{
	parse_args(argc, argv);
	int client = socket(PF_INET, SOCK_STREAM, 0);
	int len; 
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if(connect(client, (struct sockaddr*) &server, sizeof(server)) < 0)
	{
		printf("Failed to connect to the server. Exiting.....\n");
		return 0;
	} 

	printf("Connected to server on port %d.......\n", port);
	send(client, username, 1024, 0);
	send(client, password, 1024, 0);
	char response[1024]; 

	len = recv(client, response, 1024, 0);
	response[len] = '\0';

	if (strcmp(response,"correct") == 0)
	{
		printf("Authenticated, entering chatroom........\n");
	} else {
		printf("Authentication failed, shutting down....\n");
		return 0;
	}

	pthread_t thread; 

	pthread_create(&thread, NULL, receiver, (void*) &client);
	char input[1024];
	while(1)
	{
		scanf(" %1023[^\n]s",input);
		//fflush(stdin);
		//printf("Should only get here once \n");
		send(client, input, 1024, 0);
		strncat(input, "\0", 1);
		//printf("Message: %s\n", input);
		if (strcmp(input, ":exit") == 0)
		{
			printf("Exiting the program..\n");
			//pthread_cancel(thread);
			_Exit(0);
		}
	}

}