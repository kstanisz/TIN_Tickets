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
#include "Crypto.h"

Ticket* sendTicketRequest(int socket, struct sockaddr_in server_address, std::string serviceName){

	// Pobranie hasla od uzytkownika
	std::string password;	
	printf("Podaj hasło:\n");
	std::cin>>password; 
	printf("\n");

	// Szyfrowanie hasła
	std::string encryptedPassword = Crypto::instance()->rsaPublicEncrypt(password);
	encryptedPassword = Crypto::instance()->base64_encode(encryptedPassword);
		
	// Tworzenie żądania
	Request* request =  new Request(true,"",encryptedPassword, "", 0 , serviceName , "");
	std::string serializeRequest = request->serialize();
	const char* message = serializeRequest.c_str();
	   
    // Wysłanie wiadomości do serwera
	if(sendto(socket, message, strlen(message), 0, (struct sockaddr *) &server_address, sizeof(server_address) ) == -1){
		perror("Writing server");
		return nullptr;
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
			std::cout<<"Password: "<<ticket->password<<std::endl;
			read_result= 0;
			
			return ticket;
		}
	}
	while(read_result!=0);
	
	return nullptr;

}
