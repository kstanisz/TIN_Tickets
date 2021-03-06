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

void udpTime(int socket, struct sockaddr_in server_address, Ticket* ticket){
	
	// Utworzenie żądania
	Request* request =  new Request(false,ticket->ip,"", ticket->expiryDateTimestamp, "UDP_TIME","");
	std::string serializedRequest = request->serialize();
	const char* message = serializedRequest.c_str();
		
	if(sendto(socket, message, strlen(message), 0, (struct sockaddr *) &server_address, sizeof(server_address) ) == -1)	{
		perror("Writing server");
		return;
	}
      		

	// Odbiór wiadomości z serwera
	char message_from_server[4096];
	memset(message_from_server, '\0', sizeof message_from_server);
	int read_result;
	do {

		read_result= recv(socket,&message_from_server, sizeof(message_from_server),0);
	 
		if(read_result>0)
		{
			json j;
			std::string message = message_from_server;
			try{
				j = json::parse(message);
			}catch(std::exception e){
				std::cout<<"Błąd podczas parsowania wiadomości od serwera"<<std::endl;
				return;
			}
			
			Response* response;
			try{
				response = Response::deserialize(j);
			}catch(std::exception e){
				std::cout<<"Błąd podczas deserializacji wiadomości od serwera"<<std::endl;
				return;
			}
			
			if(response->getMessage() != ""){
				std::cout<<"\nOdpowiedź serwera: "<<response->getMessage()<<std::endl;
				return;
			}else{
				std::cout<<"\nOdpowiedź serwera: Pusta wiadomość."<<std::endl;
				return;
			}
		}
	}
	while(read_result!=0);
}
