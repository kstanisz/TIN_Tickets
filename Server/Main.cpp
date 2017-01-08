#include<iostream>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <fstream>

#include <set>
#include <map>

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


#include"TicketDao.h"


void serverLoop(int udpSocket, int tcpSocket){
	
	
	
	std::set <int> clientFDs;
	
	while(1){
		// zeruj zbiór deskryptorach
		fd_set fds;
		FD_ZERO(&fds);
		// dodanie deskryptorów
		FD_SET(tcpSocket, &fds);
		FD_SET(udpSocket, &fds);
	
		
		int max = std::max(udpSocket,tcpSocket);
		for (auto fd : clientFDs) {
			FD_SET (fd, &fds);
			max = std::max(fd, max);
		}
		
		int nfds = select(max+1, &fds, nullptr, nullptr, nullptr);
		if(nfds == -1){
			perror("select() error");
		}
			
		if(nfds == 0){
			// handle timeout
		}
		
		if(FD_ISSET(tcpSocket,&fds)){
			struct sockaddr address;
			socklen_t addressSize = sizeof(address);
			int newFd = accept(tcpSocket,&address,&addressSize);
			if(newFd == -1){
				perror("Tcp accept()");
				continue;
			}
			
			clientFDs.insert(newFd);
		}
		
		if(FD_ISSET(udpSocket,&fds)){
			char buf[4096];
			int numBytes = recv(udpSocket,buf,sizeof(buf),0);
			
			std::cout<<"UDP_RECEIVE"<<std::endl;
			std::cout<<buf<<std::endl;
		}
		
		for(auto fd : clientFDs){
			if(!FD_ISSET(fd,&fds))
				continue;
			
			char buf[4096];
			int numBytes = recv(fd,buf,sizeof(buf),0);
			if(numBytes > 0){
				std::cout<<"TCP_RECEIVE"<<std::endl;
				std::cout<<buf<<std::endl;
			}else{
				close(fd);
				clientFDs.erase(fd);
			}
			
		}
	}
}


int main(){
	
	// Utworzenie socketów i ich adresów oraz bind.
	
	const int udpSocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(udpSocket == -1){
		perror("Error creating udp socket.");
		exit(1);
	}
	
	struct sockaddr_in udp_address;
	udp_address.sin_family = AF_INET;
	udp_address.sin_addr.s_addr = INADDR_ANY;
	udp_address.sin_port = htons(9001);
	
	if(bind(udpSocket, (const struct sockaddr *) & udp_address, sizeof(udp_address)) == -1){
		perror("Error binding udp socket.");
		exit(1);
	}
	
	const int tcpSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(tcpSocket == -1){
		perror("Error creating tcp socket.");
		exit(1);
	}
			
	struct sockaddr_in tcp_address;
	tcp_address.sin_family = AF_INET;
	tcp_address.sin_addr.s_addr = INADDR_ANY;
	tcp_address.sin_port = htons(9002);
	
		
	if(bind(tcpSocket, (const struct sockaddr *) & tcp_address, sizeof(tcp_address)) == -1){
		perror("Error binding tcp socket.");
		exit(1);
	}
	
	// Ustawienie socketa TCP na nasłuchiwanie
	
	listen(tcpSocket,999);
	
	serverLoop(udpSocket,tcpSocket);
	
	return 0;
}