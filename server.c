#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Networking header files 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//Thread Header files 
#include <pthread.h>


pthread_mutex_t mutex;
pthread_t tID[2];
pthread_cond_t cv; 

#define CLIENTS 10
#define PORT  4455 


void init(){
	printf("trying to create a server");
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cv,NULL);
}
char* processRequest(char* req){
	char* response = (char*)malloc(1024*sizeof(char));
	for(int i=0; i<6;i++)
		*(response+i)='t';
	*(response+6)='\0';		
	return response; 
}

void* connection_handler(void*);

int main(void){
	init();
	int sockfd; 
	struct sockaddr_in serverAdd;

	int newSocket;
	struct sockaddr_in newAdd; 

	socklen_t addr_size; 
	char buffer[1024];

	sockfd=socket(PF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		perror("Failed to create socket \n");
		exit(0);
	}
	printf("Socket created \n");

	memset(&serverAdd,'\0',sizeof(serverAdd));
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_port = htons(PORT);
	serverAdd.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(bind(sockfd,(struct sockaddr*)&serverAdd, sizeof(serverAdd))){
		perror("Bind Failed \n");
		exit(0);
	}
	printf("Bind to socket\n");

	listen(sockfd,CLIENTS);
	printf("Listening...\n");
	addr_size=sizeof(newAdd);

	while(newSocket=accept(sockfd,(struct sockaddr*)&newAdd,&addr_size)){
		printf("Connection Accepted \n");
		pthread_t newThread; 
	int*	newSock=malloc(1);
		*newSock = newSocket;
		if(pthread_create(&newThread,NULL,connection_handler,(void*)newSock)){
			perror("Could not create Thread \n");
			return 1;
		}		
		if(newSocket<0){
			perror("accept Failed");
			return 1; 
		}	
	}
	return 0;
}

void* connection_handler(void* socket){
	int sock = *(int*)socket;
	int readSize;
	sleep(1);
	char* message;
	char clientMessage[1024];
	message="Welcome to server";
	write(sock,message,strlen(message));
	while(readSize=recv(sock,clientMessage,sizeof(clientMessage)-1,0)){
		clientMessage[readSize] ='\0';
		printf("Request Received : %s\n", clientMessage);		
		write(sock,processRequest(clientMessage),strlen(clientMessage));
		memset(clientMessage,'\0',1024);
	}
	if(readSize==0){
		printf("Client Disconnected\n");
		fflush(stdout);
	}else if(readSize <0){
		perror("Error Received");
	}
	free(socket);
	return 0;
}
