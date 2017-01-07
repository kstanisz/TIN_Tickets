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

void tcpEcho(int socket, struct sockaddr_in server_address){

	using namespace std;
	int sock;
	sock = socket;

	int connection;
	connection = connect(sock, (struct sockaddr*) &server_address, sizeof(server_address));
		 
        if(connection ==-1){
      		perror("Connect server error");
      		exit(1);
    	};

	std::string password;
	std::string name = "localhost"; 				//to_string(server_address.sin_addr.s_addr);
	std::cout<<name;
	std::cout<<endl;
	
	std::cout<<"Write your password:"<<std::endl;
	std::cin>>password; 
	std::cout<<endl;

	std::string number_of_service = "3";
	
	std:string all_details; 
    	all_details = name + password + number_of_service;
    	cout << "Polaczony tekst to: \"" << all_details << "\"" << endl;


	char * message = new char[ all_details.size() + 1 ];
	strcpy( message, all_details.c_str() );
	


  	if(write(sock, message, sizeof(message))==-1)
      		perror("writing server");
  	else {	
        
          printf("You successfully sent a message.\n");
         
	}

	delete[] message;
}


