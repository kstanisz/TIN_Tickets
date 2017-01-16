#include <iostream>
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

#include "json.hpp"
#include "TicketDao.h"
#include "DataStructures.h"
#include "Crypto.h"

using json = nlohmann::json;

void serverLoop(int udpSocket, int tcpSocket, TicketDao* ticketDao);
void processUdp(int fd, TicketDao* ticketDao);
void processTcp(int fd, std::string ip, std::map<int,std::string>* clientFDs, TicketDao* ticketDao);
Response* releaseTicket(Request* request, std::string ip, TicketDao* ticketDao);
Response* runUdpService(Request* request, std::string ip, TicketDao* ticketDao);
Response* runTcpService(Request* request, std::string ip, TicketDao* ticketDao);

int main(){

	TicketDao* ticketDao = new TicketDao();
	ticketDao->connect();
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
	
	serverLoop(udpSocket,tcpSocket,ticketDao);
	
	ticketDao->disconnect();
	delete ticketDao;
	
	return 0;
}

void serverLoop(int udpSocket, int tcpSocket, TicketDao* ticketDao){
	
	std::map <int,std::string> clientFDs;
	
	while(1){
		
		// zeruj zbiór deskryptorach
		fd_set fds;
		FD_ZERO(&fds);
		// dodanie deskryptorów
		FD_SET(tcpSocket, &fds);
		FD_SET(udpSocket, &fds);
	
		
		int max = std::max(udpSocket,tcpSocket);
		for (auto& kv : clientFDs) {
			int fd = kv.first;
			FD_SET (fd, &fds);
			max = std::max(fd, max);
		}
		
		int nfds = select(max+1, &fds, nullptr, nullptr, nullptr);
		if(nfds == -1){
			perror("select() error");
		}
			
		if(nfds == 0){
			perror("select() timout");
		}
		
		if(FD_ISSET(tcpSocket,&fds)){
			struct sockaddr_in address;
			socklen_t addressSize = sizeof(address);
			int newFd = accept(tcpSocket,(sockaddr*) &address,&addressSize);
			if(newFd == -1){
				perror("Tcp accept()");
				continue;
			}
			
			clientFDs[newFd] = inet_ntoa(address.sin_addr);
		}
		
		if(FD_ISSET(udpSocket,&fds)){
			processUdp(udpSocket,ticketDao);
		}
		
		for(auto& kv : clientFDs){
			
			int fd = kv.first;
			std::string ip = kv.second;
			
			if(!FD_ISSET(fd,&fds))
				continue;
					
			processTcp(fd,ip, &clientFDs, ticketDao);
		}
	}
}

void processUdp(int fd, TicketDao* ticketDao)
{
	char buf[4096];
	sockaddr_in from;
	socklen_t fromSize = sizeof from;
	int numBytes = recvfrom(fd,buf,sizeof(buf),0,(sockaddr*) &from, &fromSize);

	json j;
	std::string message = buf;
	try{
		j = json::parse(message);
	}catch(std::exception e){
		//std::cout<<"Error parsing json."<<std::endl;
	}
			
	Request* request;
			
	try{
		request = Request::deserialize(j);
	}catch(std::exception e){
		//std::cout<<"Error deserializing json."<<std::endl;
	}
			
	std::string ip = inet_ntoa(from.sin_addr);
	Response * response;
	if(request->isReleaseTicket()){
		request -> setIp(ip);
		response = releaseTicket(request,ip,ticketDao);
		std::string message = response->serialize();
		sendto(fd,message.c_str(),strlen(message.c_str()),0,(sockaddr* ) &from , sizeof from);
	}else{
		response = runUdpService(request,ip,ticketDao);
		sendto(fd,response->getMessage().c_str(),response->getMessage().size(),0,(sockaddr* ) &from , sizeof from);
	}
}

void processTcp(int fd, std::string ip, std::map<int,std::string>* clientFDs, TicketDao* ticketDao)
{
	char buf[4096];
	int numBytes = read(fd,buf,sizeof(buf));
	if(numBytes > 0){
		json j;
		std::string message = buf;
		try{
			j = json::parse(message);
		}catch(std::exception e){
			//std::cout<<"Error parsing json."<<std::endl;
		}
		
		Request* request;
		
		try{
			request = Request::deserialize(j);
		}catch(std::exception e){
			//std::cout<<"Error deserializing json."<<std::endl;
		}
				
		Response * response = runTcpService(request,ip,ticketDao);
		write(fd, response->getMessage().c_str(), strlen(response->getMessage().c_str()));
				
		}else{
			close(fd);
			(*clientFDs).erase(fd);
		}	
}

Response* releaseTicket(Request* request, std::string ip, TicketDao* ticketDao)
{
	std::cout<<"Serwer wykonuje wydanie biletu"<<std::endl;
	std::string password = request->getPassword();
	std::string serviceName = request->getServiceName();
	
	if(!ticketDao->checkAccess(ip,password,serviceName)){
		Ticket dummy = Ticket();
		return new Response("Brak upoważnienia do wydania biletu na usługę "+serviceName,&dummy);
	}
	
	Ticket* ticket = ticketDao->releaseTicket(ip,request->getPassword(),request->getServiceName());
	
	return new Response("",ticket);
}

Response* runUdpService(Request* request, std::string ip, TicketDao* ticketDao)
{
	std::string serviceName = request->getServiceName();
	std::cout<<"Serwer wykonuje usługę: "+serviceName<<std::endl;
	Ticket dummy = Ticket();
	bool ticketValid = ticketDao->checkTicket(request->getIp(),request->getServiceName());
	if(!ticketValid){
		return new Response("Bilet nieważny!",&dummy);
	}
	
	if(serviceName == "UDP_ECHO"){
		return new Response(request->getMessage(),&dummy);
	}else if(serviceName == "UDP_TIME"){
		return new Response(request->getMessage(),&dummy);
	}else{
		std::cout<<"Niepoprawna nazwa usługi"<<std::endl;
	}
}

Response* runTcpService(Request* request, std::string ip, TicketDao* ticketDao)
{
	std::string serviceName = request->getServiceName();
	std::cout<<"Serwer wykonuje usługę: "+serviceName<<std::endl;
	Ticket dummy = Ticket();
	
	bool ticketValid = ticketDao->checkTicket(request->getIp(),request->getServiceName());
	if(!ticketValid){
		return new Response("Bilet nieważny!",&dummy);
	}
	
	
	if(serviceName == "TCP_ECHO"){
		return new Response(request->getMessage(),&dummy);
	}else{
		std::cout<<"Niepoprawna nazwa usługi"<<std::endl;
	}
}
