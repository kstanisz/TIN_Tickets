#include "TicketDao.h"

TicketDao::TicketDao()
{
	
}

TicketDao::~TicketDao()
{
	
}

bool TicketDao::connect()
{	
	mysql_init(&mysql);

	// host, user, password, schema
	return mysql_real_connect(&mysql, "127.0.0.1", "dev", "password", "tickets", 0, NULL, 0);	
}

void TicketDao::disconnect()
{
	mysql_close(&mysql);
}

bool TicketDao::checkAccess(std::string ip, std::string password, std::string serviceName)
{
	MYSQL_RES* queryId;	
	std::stringstream query;
	query << "SELECT * FROM USER_HAS_SERVICE US " <<
						"LEFT JOIN USER U ON U.id = US.user_id " <<
						"LEFT JOIN SERVICE S ON S.id = US.service_id " <<
						"WHERE U.ip='"<<ip<<"' "<<
						"AND U.password='"<<password<<"' "<<
						"AND S.name='"<<serviceName<<"'";
						
	mysql_select_db(&mysql,"tickets");
	mysql_query(&mysql,query.str().c_str());
	queryId = mysql_store_result(&mysql);
	
	return(mysql_fetch_row(queryId) != NULL);
}

bool TicketDao::checkTicket(std::string ip, std::string serviceName)
{
	MYSQL_RES* queryId;	
	std::stringstream query;
	query << "SELECT * FROM TICKET T " <<
						"LEFT JOIN USER U ON U.id = T.user_id " <<
						"LEFT JOIN SERVICE S ON S.id = T.service_id " <<
						"WHERE U.ip='"<<ip<<"' "<<
						"AND S.name='"<<serviceName<<"' "<<
						"AND T.expiry_date > sysdate()";
						
	mysql_select_db(&mysql,"tickets");
	mysql_query(&mysql,query.str().c_str());
	queryId = mysql_store_result(&mysql);
	
	return(mysql_fetch_row(queryId) != NULL);
}

Ticket* TicketDao::releaseTicket(std::string ip, std::string password, std::string serviceName)
{
	
	Ticket* ticket = new Ticket();
	ticket->ip = ip;
	ticket->serviceName = serviceName;
	
	int checksum = 0;
	
	//TODO: checksum, expiry_date
	
	if(checkTicket(ip,serviceName)){
		std::stringstream query;
		// TODO: query
		query << "UPDATE TICKET SET EXPIRY_DATE = '2017-7-08' ";
				
		if(mysql_query(&mysql,query.str().c_str())){

		}
	}else{
		std::stringstream query;
		query << "INSERT INTO TICKET(USER_ID,SERVICE_ID,CHECKSUM,EXPIRY_DATE) "<<
				"SELECT U.ID, S.ID,"<<"1"<<", '2017-7-04' "
				"FROM USER U, SERVICE S "<< 
				"WHERE U.IP='"<<ip<<"'AND U.PASSWORD='"<<password<<"' AND S.NAME='"<<serviceName<<"'";
				
		if(mysql_query(&mysql,query.str().c_str())){

		}
	}
	
	return ticket;
}