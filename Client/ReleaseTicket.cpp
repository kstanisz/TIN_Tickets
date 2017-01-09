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


	char message_from_server[4096];
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
				std::cout<<"Error parsing json."<<std::endl;
			}
			
			Response* response;
			try{
				response = Response::deserialize(j);
			}catch(std::exception e){
				std::cout<<"Error deserializing json."<<std::endl;
			}
			
			if(response->getMessage() != ""){
				std::cout<<"\nOdpowiedź serwera: "<<response->getMessage()<<std::endl;
				return nullptr;
			}
			
			Ticket* ticket = response->getTicket();
			std::cout<<"\nSerwer wydał bilet:"<<std::endl;
			std::cout<<"Ip: "<<ticket->ip<<std::endl;
			std::cout<<"Service name: "<<ticket->serviceName<<std::endl;
			read_result= 0;
			
			return ticket;
		}
	}
	while(read_result!=0);
	
	return nullptr;

}
