#ifndef TICKETDAO_H_
#define TICKETDAO_H_

#include <iostream>
#include <sstream>
#include <string>
#include <mysql.h>

#include "DataStructures.h"
#include "Crypto.h"

class TicketDao
{
	private:
		MYSQL mysql;
	public:
		TicketDao();
		~TicketDao();
		
		bool connect();
		void disconnect();
		
		bool checkAccess(std::string ip, std::string password, std::string serviceName);
		bool checkTicket(std::string ip, std::string serviceName);
		Ticket* releaseTicket(std::string ip, std::string password, std::string serviceName);
};

#endif //TICKETDAO_H_
