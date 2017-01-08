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
// Operacja wydawania biletów
void releaseTicket(int socket, struct sockaddr_in server_address);
// Wywołanie serwisu UDP ECHO
void udpEcho(int socket, struct sockaddr_in server_address);
// Wywołanie serwisu UDP TIME
void udpTime(int socket, struct sockaddr_in server_address);
// Wywołanie serwisu TCP ECHO
void tcpEcho(int socket, struct sockaddr_in server_address);
// Wyslanie danych serverowi potrzebnych do wydania biletu
void sendTicketRequest(int socket, struct sockaddr_in server_address, int number_of_service);


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

	printInfo();
	
	while(1){
		int x;
		printf("\n");
		scanf("%d", &x);
	
		switch(x){
			case 0:
				printf("Koniec działania programu\n");
				break;
			case 1:
				releaseTicket(udp_sock, udp_server_address);
				break;
			case 2:
				udpEcho(udp_sock, udp_server_address);
				break;
			case 3:
				udpTime(udp_sock, udp_server_address);
				break;
			case 4:
				tcpEcho(tcp_sock, tcp_echo_server_address);
				break;
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
	printf("<---TICKETS_CLIENT--->\n"
	" 1) Wydaj bilet na usługę.\n"
	" 2) Wykonaj usługę UDP echo.\n"
	" 3) Wykonaj usługę UDP czas.\n"
	" 4) Wykonaj usługę TCP echo.\n"
	" 0) Zakończ program.\n");
}

void printServiceChoiceInfo(){
	printf("<---WYBIERZ USŁUGĘ--->\n"
	" 1) UDP ECHO.\n"
	" 2) UDP CZAS.\n"
	" 3) TCP ECHO.\n");
}

void releaseTicket(int socket, struct sockaddr_in server_address){
	printServiceChoiceInfo();
	int choice;
	scanf("%d", &choice);
	switch(choice){
		case 1:
			printf("Wybrano UDP ECHO\n");
			sendTicketRequest(socket, server_address, 1);
			break;
		case 2:
			printf("Wybrano UDP CZAS\n");
			sendTicketRequest(socket, server_address, 2);
			break;
		case 3:
			printf("Wybrano TCP ECHO\n");
			sendTicketRequest(socket, server_address, 3);
			break;
		default:
			printf("Niepoprawny numer usługi!\n");
			break;
	}
}




