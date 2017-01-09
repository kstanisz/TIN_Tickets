#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "json.hpp"
#include "DataStructures.h"

using json = nlohmann::json;

void udpEcho(int socket, struct sockaddr_in server_address, Ticket* ticket){
	
	Request* request =  new Request(false,"","","UDP_ECHO","HELLO_WORLD!");
	const char* message = request->serialize().c_str();
	
	if(sendto(socket, message, strlen(message), 0, (struct sockaddr *) &server_address, sizeof(server_address) ) == -1)			
      		perror("writing server");

	char message_from_server[4096];
	int read_result;
	do {

		read_result= read(socket,&message_from_server, sizeof(message_from_server));
	 
		if(read_result>0)
		{
			printf("\nOdpowiedź serwera: %s\n", message_from_server);
			read_result= 0;
		}
	}
	while(read_result!=0);
}