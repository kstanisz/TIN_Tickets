#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "DataStructures.h"
void tcpEcho(int socket, struct sockaddr_in server_address, Ticket* ticket){


	Request* request =  new Request(false,"","","TCP_ECHO","HELLO_WORLD!");
	const char* message = request->serialize().c_str();
	
  	if(write(socket, message, strlen(message))==-1)
      		perror("writing server");
  	else {	
        
          printf("You successfully sent a message.\n");
         
	}

	char message_from_server[4096];
	int read_result;
	do {

		read_result= read(  socket,   &message_from_server, sizeof(message_from_server));
	 
		if(read_result>0)
		{
			printf("to jest info od klienta: %s\n", message_from_server);
			read_result= 0;
		}
	}
	while(read_result!=0);

}


