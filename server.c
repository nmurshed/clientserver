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


#define DEBUG 0

struct Request{
	char clientMessage[1024]; 
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
	char tempMessage[1024];
	int read=0;
	char* welcome = "Passed";
	memset(tempMessage,'\0',sizeof(tempMessage)); 

	while(newSocket=accept(sockfd,(struct sockaddr*)&newAdd,&addr_size)){
		printf("Connection Accepted \n");
		write(newSocket,welcome,sizeof(welcome));
	    memset(tempMessage,'\0',sizeof(tempMessage));

		pthread_mutex_lock(&mutex);
		
		while((head+1)%1024==tail){
			printf("Waiting for empty queue\n");
			pthread_cond_wait(&empty,&mutex);
		}


		//pthread_mutex_lock(&mutex);

	    while(read = recv(newSocket,tempMessage,sizeof(tempMessage)-1,0)){
	    tempMessage[read]='\n';
	    if(read<0){
	    	perror("Error Received \n");
	    }else if(read==0){
	    	perror("Client Disconnected \n");
			fflush(stdout);
	    }
	    else{
	    	
	    	strncpy(ClientRequests[head].clientMessage,tempMessage,sizeof(tempMessage));
			ClientRequests[head].clientAddress = newAdd;
			head=(head+1)%1024; 
			
	    }
		
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&mutex);
		if(DEBUG){
			printf("message  %s \n", ClientRequests[head-1].clientMessage);
			printf("ipAddress %s\n",inet_ntoa(ClientRequests[head-1].clientAddress.sin_addr));
			printf("portNumber %d\n",ClientRequests[head-1].clientAddress.sin_port);
		}	
	}


	}
		
	
	
	return 0;
}

int push(struct Request req){
	return 0; 
}

void* processQueue(){
	while(1){
		pthread_mutex_lock(&mutex);
		while(head==tail){
			printf("waiting to process request\n");
			pthread_cond_wait(&full,&mutex);
		}

		printf("Processing : message  %s \n", ClientRequests[tail].clientMessage);
		printf("Processing :ipAddress %s\n",inet_ntoa(ClientRequests[tail].clientAddress.sin_addr));
		printf("Processing : portNumber %d\n",ClientRequests[tail].clientAddress.sin_port);
		printf("Processing : Thread ID : %lu \n",pthread_self());
		tail=(tail+1)%1024; 
	
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&mutex);
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
