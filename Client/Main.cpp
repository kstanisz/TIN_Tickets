#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

// Wyświetlanie informacji przy wyborze akcji w interfejsie użytkownika
void printInfo();
// Wyświetlanie informacji przy wyborze serwisu dla wydania biletu.
void printServiceChoiceInfo();
// Operacja wyydawania biletów
void releaseTicket(int socket, struct sockaddr_in server_address);
// Wywołanie serwisu UDP ECHO
void udpEcho(int socket, struct sockaddr_in);
// Wywołanie serwisu UDP TIME
void udpTime(int socket, struct sockaddr_in);
// Wywołanie serwisu TCP ECHO
void tcpEcho(int socket, struct sockaddr_in);


int main(){

	// UTWORZENIE SOCKETÓW

	int tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_sock == -1){
		std::cout<<"Nie udało się utworzyć tcp_socekt."<<std::endl;
		exit(1);
	}
	
	int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_sock == -1) {
		std::cout<<"Nie udało się utworzyć udp_socket."<<std::endl;
		exit(1);
	}

	// ADRESY SOCKETÓW SERWEROWYCH
	
	struct sockaddr_in udp_release_ticket_server_address;
	udp_release_ticket_server_address.sin_family = AF_INET;
	udp_release_ticket_server_address.sin_addr.s_addr = INADDR_ANY;
	udp_release_ticket_server_address.sin_port = htons(9001);

	struct sockaddr_in udp_echo_server_address;
	udp_echo_server_address.sin_family = AF_INET;
	udp_echo_server_address.sin_addr.s_addr = INADDR_ANY;
	udp_echo_server_address.sin_port = htons(9002);
	
	struct sockaddr_in udp_time_server_address;
	udp_time_server_address.sin_family = AF_INET;
	udp_time_server_address.sin_addr.s_addr = INADDR_ANY;
	udp_time_server_address.sin_port = htons(9003);
		
	struct sockaddr_in tcp_echo_server_address;
	tcp_echo_server_address.sin_family = AF_INET;
	tcp_echo_server_address.sin_addr.s_addr = INADDR_ANY;
	tcp_echo_server_address.sin_port = htons(9004);

	// USER INTERFACE
	int x;
	printInfo();
	while(std::cin>>x){
		switch(x){
			case 0:
				std::cout<<"Koniec działania programu"<<std::endl;
				break;
			case 1:
				releaseTicket(udp_sock, udp_release_ticket_server_address);
				break;
			case 2:
				udpEcho(udp_sock, udp_echo_server_address);
				break;
			case 3:
				udpTime(udp_sock, udp_time_server_address);
				break;
			case 4:
				tcpEcho(tcp_sock, tcp_echo_server_address);
				break;
			default:
				std::cout<<"Niepoprawny wybór!."<<std::endl;
		}
		
		if(x==0){
			break;
		}
		
		printInfo();
	}
	
	return 0;
}

void printInfo(){
	std::cout<<"<---TICKETS_CLIENT--->"<<std::endl;
	std::cout<<" 1) Wydaj bilet na usługę."<<std::endl;
	std::cout<<" 2) Wykonaj usługę UDP echo."<<std::endl;
	std::cout<<" 3) Wykonaj usługę TCP echo."<<std::endl;
	std::cout<<" 4) Wykonaj usługę UDP czas."<<std::endl;
	std::cout<<" 0) Zakończ program."<<std::endl;
}

void printServiceChoiceInfo(){
	std::cout<<"<---WYBIERZ USŁUGĘ--->"<<std::endl;
	std::cout<<" 1) UDP ECHO."<<std::endl;
	std::cout<<" 2) UDP CZAS."<<std::endl;
	std::cout<<" 3) TCP ECHO."<<std::endl;
}

void releaseTicket(int socket, struct sockaddr_in server_address){
	printServiceChoiceInfo();
	int choice;
	std::cin>>choice;
	switch(choice){
		case 1:
			std::cout<<"Wybrano UDP ECHO"<<std::endl;
			break;
		case 2:
			std::cout<<"Wybrano UDP CZAS"<<std::endl;
			break;
		case 3:
			std::cout<<"Wybrano TCP ECHO"<<std::endl;
		
			break;
		default:
			std::cout<<"Niepoprawny numer usługi!"<<std::endl;
			break;
	}
}

void udpEcho(int socket, struct sockaddr_in){
	 
}

void udpTime(int socket, struct sockaddr_in){
	
}


