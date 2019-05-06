#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	char* response;
	//do something with response 
	return response; 
}

int main(void){
	init();
	int sockfd; 
	struct sockaddr_in serverAdd;

	int newSocket;
	struct sockaddr_in newAdd; 

	socklen_t addr_size; 
	char buffer[1024];

	sockfd=socket(PF_INET,SOCK_STREAM,0);
	memset(&serverAdd,'\0',sizeof(serverAdd));
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_port = htons(PORT);
	serverAdd.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(sockfd,(struct sockaddr*)&serverAdd, sizeof(serverAdd));

	listen(sockfd,CLIENTS);
	addr_size=sizeof(newAdd);

	newSocket=accept(sockfd,(struct sockaddr*)&newAdd,&addr_size);

	strcpy(buffer,"hello");
	send(newSocket,buffer,strlen(buffer),0);

	

	
	return 0;
}