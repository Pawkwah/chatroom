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
#include <time.h>

int port = 5001; 
int clientCount = 0;
static pthread_mutex_t thread = PTHREAD_MUTEX_INITIALIZER;

struct client 
{
	struct sockaddr_in clientAddr;
	int len;
	int index; 
	int socketID;
	char username[15];
	int verified; 
} typedef client_info_t;

client_info_t clients[1024];
pthread_t threads[1024];

int broadcast_some(char data[], int not)
{
	for (int x = 0; x < clientCount; x++)
	{
		if (clients[x].verified && x != not)
		{
			send(clients[x].socketID, data, 1055, 0);
		}
	}
}

int broadcast(char data[])
{
	for (int x = 0; x < clientCount; x++)
	{
		if (clients[x].verified)
		{
			send(clients[x].socketID, data, 1055, 0);
		}
	}
}
int startChat(void* client)
{
	time_t rawtime; 
	struct tm * timeinfo;
	char message[1055];
	char data[1024];
	int len; 
	int all = 0;
	client_info_t* clientInfo = (client_info_t*) client;
	//printf("Username: %s\n", clientInfo->username);
	printf("Client %s successfully entered the chat.\n", clientInfo->username);
	while(1)
	{
		len = recv(clientInfo->socketID, data, 1024, 0);
		data[len] = '\0';
		printf("received message: %s\n", data);
		if (data[0] == ':')
		{
			
			if (!strcmp(data, ":)")){
				strcpy(data, "[feeling happy]");
				all = 1;
			}
			else if(!strcmp(data, ":(")){
				strcpy(data, "[feeling sad]");
				all = 1;
			}
			else if(!strcmp(data, ":mytime")){
				time ( &rawtime );
				timeinfo = localtime ( &rawtime );
				snprintf(data, 1055, "Current local time and date: %s", asctime (timeinfo));
				all = 1;
			}
			else if(!strcmp(data, ":+1hr")){
				time ( &rawtime );
				rawtime += (60*60);
				timeinfo = localtime ( &rawtime );
				snprintf(data, 1055, "Current local time and date + 1 hour: %s", asctime (timeinfo));
				all = 1;
			}
			else if(!strcmp(data, ":exit"))
			{
				snprintf(message, 1055, "%s has left the chat.", clientInfo->username);
				clientInfo->verified = 0;
				goto send;
			}
		}
	
		snprintf(message, 1055, "%s: %s", clientInfo->username, data);
		
		//if exit command, shut down thread and make client not verified
		if (all)
		{
			printf("Broadcasting message: %s\n", message);
			broadcast(message);
			all = 0;
		} else {
		send:
			printf("Broadcasting message: %s\n", message);
			broadcast_some(message, clientInfo->index);
			if (clientInfo->verified == 0)
			{
				pthread_exit(NULL);
			}
	}
		}
	
	
}

void * verification(void * client)
{
	//printf("Got to verification\n");
	client_info_t* clientInfo = (client_info_t*) client;
	char password[1024]; 
	char data[1024];
	char output[1024];
	int len = recv(clientInfo->socketID, data, 1024, 0);
	//printf("Got to verification\n");
	data[len] = '\0';
	//printf("DATA: %s\n", data);
	strcpy(clientInfo->username, data); 
	len = recv(clientInfo->socketID, password, 1024, 0);
	//printf("Got to verification\n");
	password[len] = '\0';
	if (strcmp(password, "cs3251secret") != 0)
	{
		strcpy(output, "wrong");
		send(clientInfo->socketID, output, 1024, 0);
		pthread_exit(NULL);
		//len = recv(clientInfo->socketID, password, 1024, 0);
		//password[len] = '\0';
	}
	strcpy(output,"correct");
	send(clientInfo->socketID, output, 1024, 0);
	clientInfo->verified = 1; 
	//broadcast that someone has entered the chat 
	//printf("Username: %s\n", clientInfo->username);
	snprintf(data, 1024, "%s has entered the chat.", clientInfo->username);
	broadcast(data);
	startChat((void *) clientInfo);
}


void parse_args(int argc, char *argv[])
{
	int opt;
	while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
        case 'p':
            port = atoi(optarg);
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[])
{
	//printf("1\n");
	parse_args(argc, argv);
	int server = socket(PF_INET, SOCK_STREAM, 0);
	
	//printf("1\n");
	struct sockaddr_in serverAddr; 

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
	//printf("1\n");

	if(bind(server,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) < 0)
	{
		printf("Binding failed. Server shutting down.........\n");
		return 0;
	} 

	if(listen(server, 1024) < 0)
	{
		printf("Listening failed. Server shutting down.........\n");
		return 0; 
	}

	printf("Server successfully started... listening on %d\n", port);

	while(1)
	{
		//printf("2\n");
		clients[clientCount].socketID = accept(server, (struct sockaddr*) &clients[clientCount].clientAddr, &clients[clientCount].len);
		clients[clientCount].index = clientCount;
		clients[clientCount].verified = 0; 
		//printf("2\n");
		pthread_create(&threads[clientCount], NULL, verification, (void *) &clients[clientCount]);
		clientCount++;
	}
}