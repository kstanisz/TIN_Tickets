#include <iostream>
#include <sstream>
#include <string>
#include <mysql.h>

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
		bool checkTicket(std::string ip, std::string password, std::string serviceName);
		void releaseTicket(std::string ip, std::string password, std::string serviceName);
};

