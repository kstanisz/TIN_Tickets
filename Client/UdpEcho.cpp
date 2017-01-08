#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

void udpEcho(int socket, struct sockaddr_in server_address){

	std::string password;
	std::string name = "localhost";						//server_address.sin_addr.s_addr;
	
	printf("Write your password:\n");
	std::cin>>password; 
	printf("\n");

	std::string number_of_service = "2";
	
	std::string all_details = name + password + number_of_service;

	char * message = new char[ all_details.size() + 1 ];
	strcpy( message, all_details.c_str() );
	
	//send(sock, message, 1024, 0)
	if(sendto(socket, message, 1024, 0, (struct sockaddr *) &server_address, sizeof(server_address) ) == -1) 			
      		perror("writing server");
  	else {	
        
          printf("You successfully sent a message.\n");
         
	}
	delete[] message;

	/*char message_from_server[50];
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

}
