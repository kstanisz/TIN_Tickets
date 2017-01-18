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

void TicketDao::updateTestPassword()
{
	std::string password = "password1";
	std::string salt = "2gcmCA0QhhUqkBUJTnQaAsu1kVyBHoUy4hIvh8nHrJY=";
	std::string passwordHash = Crypto::instance()->passwordSaltedHash(salt,password);
	MYSQL_RES* queryId;	
	mysql_select_db(&mysql,"tickets");
	std::stringstream query;
	query << "UPDATE USER SET PASSWORD_HASH='"<<passwordHash<<"', "<<
			"SALT='"<<salt<<"' WHERE ID =1";

	if(mysql_query(&mysql,query.str().c_str())){
		std::cout<<"Bład podczas aktualizacji haseł bazy danych!"<<std::endl;
	}
}

bool TicketDao::authenticateUser(std::string ip, std::string password)
{
	MYSQL_RES* queryId;	
	std::stringstream query;
	query << "SELECT PASSWORD_HASH, SALT FROM USER U WHERE U.IP='" <<ip<<"'";		
	mysql_select_db(&mysql,"tickets");
	mysql_query(&mysql,query.str().c_str());
	queryId = mysql_store_result(&mysql);
	
	MYSQL_ROW result = mysql_fetch_row(queryId);
	if(result == NULL){
		return false;
	}
	
	std::string passwordHash = result[0];
	std::string salt = result[1];

	std::string passwordHashCheck = Crypto::instance()->passwordSaltedHash(salt,password);
	
	return (passwordHash == passwordHashCheck);
}

bool TicketDao::checkAccessToService(std::string ip, std::string serviceName)
{
	MYSQL_RES* queryId;	
	std::stringstream query;
	query << "SELECT COUNT(*) FROM USER_HAS_SERVICE US " <<
						"LEFT JOIN USER U ON U.id = US.user_id " <<
						"LEFT JOIN SERVICE S ON S.id = US.service_id " <<
						"WHERE U.ip='"<<ip<<"' "<<
						"AND S.name='"<<serviceName<<"'";
						
	mysql_select_db(&mysql,"tickets");
	mysql_query(&mysql,query.str().c_str());
	queryId = mysql_store_result(&mysql);
	
	MYSQL_ROW result = mysql_fetch_row(queryId);
	int count = std::stoi(result[0]);
	return (count > 0);
}

bool TicketDao::checkUserHasValidTicketToService(std::string ip, std::string serviceName)
{
	MYSQL_RES* queryId;	
	std::stringstream query;
	query << "SELECT COUNT(*) FROM TICKET T " <<
						"LEFT JOIN USER U ON U.id = T.user_id " <<
						"LEFT JOIN SERVICE S ON S.id = T.service_id " <<
						"WHERE U.ip='"<<ip<<"' "<<
						"AND S.name='"<<serviceName<<"' "<<
						"AND T.expiry_date > now()";
						
	mysql_select_db(&mysql,"tickets");
	mysql_query(&mysql,query.str().c_str());
	queryId = mysql_store_result(&mysql);
	
	MYSQL_ROW result = mysql_fetch_row(queryId);
	int count = std::stoi(result[0]);
	return (count > 0);
}

bool TicketDao::checkTicket(Ticket* ticket)
{
	md_sha256_t checksum = Crypto::instance()->ticketChecksum(*ticket);
	std::string checksumString = checksum.toString();
	
	MYSQL_RES* queryId;	
	std::stringstream query;
	query << "SELECT COUNT(*) FROM TICKET T " <<
						"LEFT JOIN USER U ON U.id = T.user_id " <<
						"LEFT JOIN SERVICE S ON S.id = T.service_id " <<
						"WHERE U.ip='"<<ticket->ip<<"' "<<
						"AND S.name='"<<ticket->serviceName<<"' "<<
						"AND T.checksum='"<<checksumString<<"' "<<
						"AND T.expiry_date > now()";
						
	mysql_select_db(&mysql,"tickets");
	mysql_query(&mysql,query.str().c_str());
	queryId = mysql_store_result(&mysql);
	
	MYSQL_ROW result = mysql_fetch_row(queryId);
	int count = std::stoi(result[0]);
	return (count > 0);
}

Ticket* TicketDao::prolongTicket(std::string ip, std::string serviceName)
{
	Ticket* ticket = new Ticket();
	ticket->ip = ip;
	ticket->serviceName = serviceName;
	
	long int timestamp = static_cast<long int>(time(NULL));
	timestamp+=EXPIRY_TIME_SECONDS;
	
	ticket->expiryDateTimestamp = timestamp;

	md_sha256_t checksum = Crypto::instance()->ticketChecksum(*ticket);
	std::string checksumString = checksum.toString();
		
	MYSQL_RES* queryId;	
	mysql_select_db(&mysql,"tickets");
	std::stringstream query;
	query << "UPDATE TICKET SET EXPIRY_DATE = FROM_UNIXTIME("<<std::to_string(timestamp)<<"), CHECKSUM='"<<checksumString<<"' "<<
			" WHERE USER_ID = (SELECT ID FROM USER WHERE IP = '"<<ip<<"') AND SERVICE_ID = (SELECT ID FROM SERVICE WHERE NAME = '"<<serviceName<<"')"; 
					
	if(mysql_query(&mysql,query.str().c_str())){
		return nullptr;
	}
	
	return ticket;
}

Ticket* TicketDao::releaseTicket(std::string ip, std::string serviceName)
{
	Ticket* ticket = new Ticket();
	ticket->ip = ip;
	ticket->serviceName = serviceName;
	
	long int timestamp = static_cast<long int>(time(NULL));
	timestamp+=EXPIRY_TIME_SECONDS;
	
	ticket->expiryDateTimestamp = timestamp;

	md_sha256_t checksum = Crypto::instance()->ticketChecksum(*ticket);
	std::string checksumString = checksum.toString();
		
	MYSQL_RES* queryId;	
	mysql_select_db(&mysql,"tickets");
	std::stringstream query;
	query << "INSERT INTO TICKET(USER_ID,SERVICE_ID,CHECKSUM,EXPIRY_DATE) "<<
			"SELECT U.ID, S.ID,'"<<checksumString<<"',FROM_UNIXTIME("<<std::to_string(timestamp)<<") "
			"FROM USER U, SERVICE S "<< 
			"WHERE U.IP='"<<ip<<"' AND S.NAME='"<<serviceName<<"'";
				
	if(mysql_query(&mysql,query.str().c_str())){
		return nullptr;
	}
	
	return ticket;
}