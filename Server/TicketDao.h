#ifndef TICKETDAO_H_
#define TICKETDAO_H_

#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <mysql.h>

#include "DataStructures.h"
#include "Crypto.h"

#define EXPIRY_TIME_SECONDS 90

class TicketDao
{
	private:
		MYSQL mysql;
	public:
		TicketDao();
		~TicketDao();
		
		bool connect();
		void disconnect();
		
		void updateTestPassword();
		bool authenticateUser(std::string ip, std::string password);
		std::string getPasswordHash(std::string ip);
		bool checkAccessToService(std::string ip, std::string serviceName);
		bool checkTicket(std::string ip, std::string serviceName);
		Ticket* releaseTicket(std::string ip, std::string passwordHash, std::string serviceName);
};

#endif //TICKETDAO_H_
