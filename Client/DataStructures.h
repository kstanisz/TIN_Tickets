#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_

#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "json.hpp"

using json = nlohmann::json;

struct Ticket
{
	std::string ip;
	std::string serviceName;
	long int expiryDateTimestamp;
	std::string serialize() const;
};

class Request
{
	private:
		bool releaseTicket;
		std::string ip;
		std::string password;
		long int expiryDateTimestamp;
		std::string serviceName;
		std::string message;
		
	public:
		Request(bool releaseTicket,std::string ip, std::string password, long int expiryDateTimestamp, std::string serviceName, std::string message);
		
		bool isReleaseTicket();
		
		void setIp(std::string ip);
		
		std::string getIp();
		
		std::string getPassword();

		long int getExpiryDateTimestamp();
		
		std::string getServiceName();
		
		std::string getMessage();
		
		std::string serialize();
		
		static Request* deserialize(json j);
};

class Response
{
	private:
		std::string message;
		std::string ip;
		std::string serviceName;
		long int expiryDateTimestamp;
		
	public:
		Response(std::string message,Ticket* ticket);
		
		std::string getMessage();
		
		Ticket* getTicket();
		
		std::string serialize();
		
		static Response* deserialize(json j);
};

#endif //DATASTRUCTURES_H_
