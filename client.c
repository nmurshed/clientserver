#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Networking header files 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4455

void main(){
	int clientSocket;
	struct sockaddr_in serverAdd; 
	char buffer[1024];

	clientSocket = socket(PF_INET,SOCK_STREAM,0);
	if(clientSocket<0){
		perror("Could not create socket \n");
		exit(0);
	}
	memset(&serverAdd,'\0',sizeof(serverAdd));
	serverAdd.sin_family=AF_INET;
	serverAdd.sin_port=htons(PORT);
	serverAdd.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(clientSocket,(struct sockaddr*)&serverAdd,sizeof(serverAdd))<0){
		perror("Connection Failed");
		exit(0);
	}

	recv(clientSocket,buffer,sizeof(buffer),0);
	printf("DATA : %s\n",buffer );

	return; 			
}
