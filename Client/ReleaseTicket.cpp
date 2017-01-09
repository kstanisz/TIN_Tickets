#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "DataStructures.h"

Ticket* sendTicketRequest(int socket, struct sockaddr_in server_address, std::string serviceName){

	std::string password;	
	printf("Podaj hasło:\n");
	std::cin>>password; 
	printf("\n");
	
	Request* request =  new Request(true,"",password,serviceName,"");
	const char* message = request->serialize().c_str();
   
	if(sendto(socket, message, strlen(message), 0, (struct sockaddr *) &server_address, sizeof(server_address) ) == -1)
      		perror("writing server");
  	else {	
        
          printf("You successfully sent a message.\n");
         
	}


	/*
	char message_from_server[50];
	int read_result;
	
	do {

   		read_result= read(  sock,   &message_from_server, sizeof(message_from_server));
		printf("%d", read_result);
	 
	if(read_result>0)
		{
			printf("to jest info od klienta: %s\n", message_from_server);
			read_result= 0;
		}
	}
	while(read_result!=0);*/
	
	return nullptr;

}
