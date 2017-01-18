#include "DataStructures.h"

std::string Ticket::serialize() const {
	json j;
	j["ip"] = ip;
	j["serviceName"] = serviceName;
	j["expiryDateTimestamp"] = expiryDateTimestamp;

	return j.dump();
} 

Request::Request(bool releaseTicket,std::string ip, std::string password, long int expiryDateTimestamp, std::string serviceName, std::string message){
	this->releaseTicket = releaseTicket;
	this->ip = ip;
	this->password = password;
	this->expiryDateTimestamp = expiryDateTimestamp;
	this->serviceName = serviceName;
	this->message = message;
}

bool Request::isReleaseTicket(){
	return releaseTicket;
}

void Request::setIp(std::string ip){
	this->ip=ip;
}

std::string Request::getIp(){
	return ip;
}

std::string Request::getPassword(){
	return password;
}

long int Request::getExpiryDateTimestamp(){
	return expiryDateTimestamp;
}

std::string Request::getServiceName(){
	return serviceName;
}

std::string Request::getMessage(){
	return message;
}

std::string Request::serialize(){
	json j;
	j["releaseTicket"] = releaseTicket;
	j["ip"] = ip;
	j["password"] = password;
	j["expiryDateTimestamp"] = expiryDateTimestamp;
	j["serviceName"] = serviceName;
	j["message"] = message;
	
	return j.dump();
} 

Request* Request::deserialize(json j){
	bool releaseTicket = j["releaseTicket"];
	std::string ip = j["ip"];
	std::string password = j["password"];
	long int expiryDateTimestamp = j["expiryDateTimestamp"];
	std::string serviceName = j["serviceName"];
	std::string message = j["message"];
	
	return new Request(releaseTicket,ip,password,expiryDateTimestamp,serviceName,message);
}

Response::Response(std::string message,Ticket* ticket){
	this->message = message;
	this->ip = ticket->ip;
	this->expiryDateTimestamp = ticket->expiryDateTimestamp;
	this->serviceName = ticket->serviceName;
}

std::string Response::getMessage(){
	return message;
}

Ticket* Response::getTicket(){
	Ticket* ticket =  new Ticket();
	ticket->ip = ip;
	ticket->expiryDateTimestamp = expiryDateTimestamp;
	ticket->serviceName = serviceName;
	return ticket;
}

std::string Response::serialize(){
	json j;
	j["message"] = message;
	j["ip"] = ip;
	j["expiryDateTimestamp"] = expiryDateTimestamp;
	j["serviceName"] = serviceName;
	
	return j.dump();
}

Response* Response::deserialize(json j){
	std::string message = j["message"];
	Ticket ticket = Ticket();
	ticket.ip = j["ip"];
	ticket.expiryDateTimestamp = j["expiryDateTimestamp"];
	ticket.serviceName = j["serviceName"];
	
	return new Response(message,&ticket);
}
