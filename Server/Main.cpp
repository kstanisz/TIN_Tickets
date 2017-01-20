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

using json = nlohmann::json;

void serverLoop(int udpSocket, int tcpSocket, TicketDao* ticketDao);
void processUdp(int fd, TicketDao* ticketDao);
void processTcp(int fd, std::string ip, std::map<int,std::string>* clientFDs, TicketDao* ticketDao);
Response* releaseTicket(Request* request, std::string ip, TicketDao* ticketDao);
Response* runUdpService(Request* request, std::string ip, TicketDao* ticketDao);
Response* runTcpService(Request* request, std::string ip, TicketDao* ticketDao);

int main(){

	TicketDao* ticketDao = new TicketDao();
	
	if(!ticketDao->connect()){
		std::cout<<"Brak połączenia z bazą danych."<<std::endl;
		exit(1);
	}
	
	// Aktualizacja haseł w bazie danych.
	ticketDao->updateTestPassword();
	
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
	
	close(udpSocket);
	close(tcpSocket);
	
	return 0;
}

void serverLoop(int udpSocket, int tcpSocket, TicketDao* ticketDao){
	
	// Mapa z deskryptorami klientów TCP. (fd, sin_addr)
	std::map <int,std::string> clientFDs;
	
	while(1){
		
		fd_set fds;
		FD_ZERO(&fds);
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
			perror("Select() error");
			exit(1);
		}
			
		if(nfds == 0){
			perror("Select() timout");
			exit(1);
		}

		// TCP, dodajemy deskryptor klienta do mapy
		if(FD_ISSET(tcpSocket,&fds)){
			struct sockaddr_in address;
			socklen_t addressSize = sizeof(address);
			int newFd = accept(tcpSocket,(sockaddr*) &address,&addressSize);
			if(newFd == -1){
				perror("Tcp accept()");
				exit(1);
			}
			
			clientFDs[newFd] = inet_ntoa(address.sin_addr);
		}
		
		// UDP - PRZETWARZANIE
		if(FD_ISSET(udpSocket,&fds)){
			processUdp(udpSocket,ticketDao);
		}
		
		// TCP - PRZETWARZANIE
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
	// Czyszczenie bufora
	memset(buf, '\0', sizeof buf);
	sockaddr_in from;
	socklen_t fromSize = sizeof from;
	int numBytes = recvfrom(fd,buf,sizeof(buf),0,(sockaddr*) &from, &fromSize);

	json j;
	std::string message = buf;
	
	try{
		j = json::parse(message);
	}catch(std::exception e){
		std::string messageFromServer = "Błąd podczas parsowania wiadomości klienta!";
		std::cout<<messageFromServer<<std::endl;
		sendto(fd,messageFromServer.c_str(),strlen(messageFromServer.c_str()),0,(sockaddr* ) &from , sizeof from);
		return;
	}
			
	Request* request;
			
	try{
		request = Request::deserialize(j);
	}catch(std::exception e){
		std::string messageFromServer = "Błąd podczas deserializacji wiadomości klienta!";
		std::cout<<messageFromServer<<std::endl;
		sendto(fd,messageFromServer.c_str(),strlen(messageFromServer.c_str()),0,(sockaddr* ) &from , sizeof from);
		return;
	}
			
	std::string ip = inet_ntoa(from.sin_addr);
	Response * response;
	if(request->isReleaseTicket()){
		request -> setIp(ip);
		response = releaseTicket(request,ip,ticketDao);
	}else{
		response = runUdpService(request,ip,ticketDao);
	}
	
	std::string messageFromServer = response->serialize();
	sendto(fd,messageFromServer.c_str(),strlen(messageFromServer.c_str()),0,(sockaddr* ) &from , sizeof from);
}

void processTcp(int fd, std::string ip, std::map<int,std::string>* clientFDs, TicketDao* ticketDao)
{
	char buf[4096];
	// Czyszczenie bufora
	memset(buf, '\0', sizeof buf);
	int numBytes = read(fd,buf,sizeof(buf));
	if(numBytes > 0){
		json j;
		std::string message = buf;
		try{
			j = json::parse(message);
		}catch(std::exception e){
			std::string messageFromServer = "Błąd podczas parsowania wiadomości klienta";
			std::cout<<messageFromServer<<std::endl;
			write(fd, messageFromServer.c_str(), strlen(messageFromServer.c_str()));
			return;
		}
		
		Request* request;
		
		try{
			request = Request::deserialize(j);
		}catch(std::exception e){
			std::string messageFromServer = "Błąd podczas deserializacji wiadomości klienta";
			std::cout<<messageFromServer<<std::endl;
			write(fd, messageFromServer.c_str(), strlen(messageFromServer.c_str()));
			return;
		}
				
		Response * response = runTcpService(request,ip,ticketDao);
		std::string messageFromServer = response->serialize();
		write(fd, messageFromServer.c_str(), strlen(messageFromServer.c_str()));
				
		}else{
			close(fd);
			(*clientFDs).erase(fd);
		}	
}

Response* releaseTicket(Request* request, std::string ip, TicketDao* ticketDao)
{
	std::cout<<"Serwer wykonuje wydanie biletu."<<std::endl;
	std::string encryptedPassword = request->getPassword();
	encryptedPassword = Crypto::instance()->base64_decode(encryptedPassword);
	std::string decryptedPassword = Crypto::instance()->rsaPrivateDecrypt(encryptedPassword);
	std::string serviceName = request->getServiceName();
	
	Ticket dummy = Ticket();
		
	if(!ticketDao->authenticateUser(ip,decryptedPassword)){
		return new Response("Brak użytkownika w bazie lub niepoprawne hasło.",&dummy);
	}

	if(!ticketDao->checkAccessToService(ip,serviceName)){
		return new Response("Brak upoważnienia do wydania biletu na usługę "+serviceName,&dummy);
	}
	
	Ticket* ticket;
	if(ticketDao->checkUserHasValidTicketToService(ip,serviceName)){
		ticket = ticketDao->prolongTicket(ip,request->getServiceName());
		std::cout<<"Serwer przedłużył bilet."<<std::endl;
	}else{
		ticket = ticketDao->releaseTicket(ip,request->getServiceName());
		std::cout<<"Serwer wydał bilet."<<std::endl;
	}
	return new Response("",ticket);
}

Response* runUdpService(Request* request, std::string ip, TicketDao* ticketDao)
{
	std::string serviceName = request->getServiceName();
	std::cout<<"Serwer wykonuje usługę: "+serviceName<<std::endl;
	
	Ticket ticket = Ticket();
	ticket.ip = request->getIp();
	ticket.serviceName = request->getServiceName();
	ticket.expiryDateTimestamp = request->getExpiryDateTimestamp();
	
	if(ticket.ip != ip){
		return new Response("Adres IP nadawcy niezgodny z adresen IP biletu!",&ticket);
	}
	
	if(!ticketDao->checkTicket(&ticket)){
		return new Response("Bilet niepoprawny lub nieważny!",&ticket);
	}
	
	if(serviceName == "UDP_ECHO"){
		return new Response(request->getMessage(),&ticket);
	}else if(serviceName == "UDP_TIME"){
		std::string message="Czas: ";
		time_t rawtime;
  		struct tm * timeinfo;
  		char buffer[80];
  		time (&rawtime);
  		timeinfo = localtime(&rawtime);
  		strftime(buffer,80,"%d-%m-%Y %H:%M:%S",timeinfo);
  		std::string str(buffer);
		message += str;
		return new Response(message,&ticket);
	}else{
		return new Response("Niepoprawna nazwa usługi!",&ticket);
	}
}

Response* runTcpService(Request* request, std::string ip, TicketDao* ticketDao)
{
	std::string serviceName = request->getServiceName();
	std::cout<<"Serwer wykonuje usługę: "+serviceName<<std::endl;
	
	Ticket ticket = Ticket();
	ticket.ip = request->getIp();
	ticket.serviceName = request->getServiceName();
	ticket.expiryDateTimestamp = request->getExpiryDateTimestamp();
	
	if(ticket.ip != ip){
		return new Response("Adres IP nadawcy niezgodny z adresen IP biletu!",&ticket);
	}
	
	if(!ticketDao->checkTicket(&ticket)){
		return new Response("Bilet niepoprawny lub nieważny!",&ticket);
	}
	
	if(serviceName == "TCP_ECHO"){
		return new Response(request->getMessage(),&ticket);
	}else{
		return new Response("Niepoprawna nazwa usługi!",&ticket);
	}
}
