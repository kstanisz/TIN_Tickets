#include <iostream>
#include <stdio.h>
#include <set>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#include "DataStructures.h"

// Wyświetlanie informacji przy wyborze akcji w interfejsie użytkownika
void printInfo();
// Wyświetlanie informacji przy wyborze serwisu dla wydania biletu.
void printServiceChoiceInfo();
// Operacja wydawania biletów
Ticket* releaseTicket(int socket, struct sockaddr_in server_address);
// Wywołanie serwisu UDP ECHO
void udpEcho(int socket, struct sockaddr_in server_address, Ticket* ticket);
// Wywołanie serwisu UDP TIME
void udpTime(int socket, struct sockaddr_in server_address, Ticket* ticket);
// Wywołanie serwisu TCP ECHO
void tcpEcho(int socket, struct sockaddr_in server_address, Ticket* ticket);
// Wyslanie danych serverowi potrzebnych do wydania biletu
Ticket* sendTicketRequest(int socket, struct sockaddr_in server_address, std::string serviceName);
// Metoda sprawdzająca czy posiadany jest ważny bilet na usługę
Ticket* getTicket(std::set<Ticket> tickets, std::string serviceName);


int main(){

	// UTWORZENIE SOCKETÓW

	int tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_sock == -1){
		printf("Nie udało się utworzyć tcp_socekt.\n");
		exit(1);
	}
	
	int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_sock == -1) {
		printf("Nie udało się utworzyć udp_socket.\n");
		exit(1);
	}

	// ADRESY SOCKETÓW SERWEROWYCH
	
	struct sockaddr_in udp_server_address;
	udp_server_address.sin_family = AF_INET;
	udp_server_address.sin_addr.s_addr = INADDR_ANY;
	udp_server_address.sin_port = htons(9001);
	
		
	struct sockaddr_in tcp_echo_server_address;
	tcp_echo_server_address.sin_family = AF_INET;
	tcp_echo_server_address.sin_addr.s_addr = INADDR_ANY;
	tcp_echo_server_address.sin_port = htons(9002);

	// USER INTERFACE
	int connection = connect(tcp_sock, (struct sockaddr*) &tcp_echo_server_address, sizeof(tcp_echo_server_address));
    if(connection ==-1){
      	perror("Connect server error");
      	exit(1);
    };

	
	// BILETY
	std::set<Ticket> tickets;
	printInfo();
	
	while(1){
		int x;
		printf("\n");
		scanf("%d", &x);
	
		switch(x){
			case 0:{
				printf("Koniec działania programu\n");
				break;
			}
			case 1:{
				Ticket* ticket = releaseTicket(udp_sock, udp_server_address);
				if(ticket != nullptr){
					tickets.insert(*ticket);
				}
				break;
			}
			case 2:{
				Ticket* ticket = getTicket(tickets,"UDP_ECHO");
				if(ticket!=nullptr || true){
					udpEcho(udp_sock, udp_server_address,ticket);
				}else{
					std::cout<<"Brak ważnego biletu na usługę UDP ECHO"<<std::endl;	
				}
				break;
			}
			case 3:{
				Ticket* ticket = getTicket(tickets,"UDP_TIME");
				if(ticket!=nullptr || true){
					udpTime(udp_sock, udp_server_address,ticket);
				}else{
					std::cout<<"Brak ważnego biletu na usługę UDP TIME"<<std::endl;	
				}
				break;
			}
		
			case 4:{
				Ticket* ticket = getTicket(tickets,"TCP_ECHO");
				if(ticket != nullptr || true){
					tcpEcho(tcp_sock, tcp_echo_server_address,ticket);
				}else{
					std::cout<<"Brak ważnego biletu na usługę TCP ECHO"<<std::endl;	
				}
				break;
			}
				
			default:
				printf("Niepoprawny wybór!.\n");
		}
		
		if(x==0){
			break;
		}
		
		printInfo();
	}
	
	return 0;
}

void printInfo(){
	printf("\n\n\n<---TICKETS_CLIENT--->\n"
	" 1) Wydaj bilet na usługę.\n"
	" 2) Wykonaj usługę UDP echo.\n"
	" 3) Wykonaj usługę UDP czas.\n"
	" 4) Wykonaj usługę TCP echo.\n"
	" 0) Zakończ program.\n"
	"<------------------------------>\n");
}

void printServiceChoiceInfo(){
	printf("<---WYBIERZ USŁUGĘ--->\n"
	" 1) UDP ECHO.\n"
	" 2) UDP CZAS.\n"
	" 3) TCP ECHO.\n");
}

Ticket* releaseTicket(int socket, struct sockaddr_in server_address){
	printServiceChoiceInfo();
	int choice;
	scanf("%d", &choice);
	switch(choice){
		case 1:{
			printf("Wybrano UDP ECHO\n");
			return sendTicketRequest(socket, server_address, "UDP_ECHO");
		}
		case 2:{
			printf("Wybrano UDP CZAS\n");
			return sendTicketRequest(socket, server_address, "UDP_TIME");
		}
		case 3:{
			printf("Wybrano TCP ECHO\n");
			return sendTicketRequest(socket, server_address, "TCP_ECHO");
		}
		default:{
			printf("Niepoprawny numer usługi!\n");
			return nullptr;
		}
	}
}

Ticket* getTicket(std::set<Ticket> tickets, std::string serviceName)
{
	for(auto ticket : tickets){
		// and expiry_date <= sysdate
		if(ticket.serviceName == serviceName){
			return nullptr;
		}
	}
	
	return nullptr;
}




