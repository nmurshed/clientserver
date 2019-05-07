#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

//Networking header files 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//Thread Header files 
#include <pthread.h>

#define NTHREAD 5
pthread_mutex_t mutex;
pthread_t tID[NTHREAD];
pthread_cond_t full, empty; 

#define CLIENTS 10
#define PORT  4455 


#define DEBUG 1

struct Request{
	char* clientMessage; 
	struct sockaddr_in clientAddress; 
}ClientRequests[1024];
int head =0, tail =0; 


void* processQueue();

void init(){
	printf("trying to create a server \n");
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&full,NULL);
	pthread_cond_init(&empty,NULL);
	//Need to create N threads
	for(int i=0; i<NTHREAD; i++){
		if(pthread_create(&tID[i],NULL,processQueue,NULL)){
			perror("Could not create threads \n");
		}
	}
}
char* processRequest(char* req){
	char* response = (char*)malloc(1024*sizeof(char));
	for(int i=0; i<6;i++)
		*(response+i)='t';
	*(response+6)='\0';		
	return response; 
}

void* connection_handler(void*);

int push(struct Request);

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
	int k=0; 
	while(newSocket=accept(sockfd,(struct sockaddr*)&newAdd,&addr_size)){
		printf("Connection Accepted \n");
		pthread_mutex_lock(&mutex);
		if((head+1)%1024==tail)
			pthread_cond_wait(&empty,&mutex);
		pthread_mutex_lock(&mutex);
		ClientRequests[head].clientAddress.sin_port = newSocket;
		ClientRequests[head].clientAddress.sin_addr.s_addr = newAdd.sin_addr.s_addr;
		head++; 
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&mutex);
		if(DEBUG){
			printf("PortNumber  %d \n", newSocket);
			printf("ipAddress %s\n",inet_ntoa(newAdd.sin_addr));
		}

		if(newSocket<0){
			perror("accept Failed");
			return 1; 
		}	
	}
	return 0;
}

int push(struct Request req){
	return 0; 
}

void* processQueue(){
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
