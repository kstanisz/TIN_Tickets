#include "TicketDao.h"

TicketDao::TicketDao() : DATABASE_NAME("tickets")
{
	
}

TicketDao::~TicketDao()
{
	
}

bool TicketDao::connect()
{	
	mysql_init(&mysql);

	// host, user, password, schema
	return mysql_real_connect(&mysql, "127.0.0.1", "dev", "password", DATABASE_NAME.c_str(), 0, NULL, 0);	
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
	mysql_select_db(&mysql,DATABASE_NAME.c_str());
	std::stringstream query;
	query << "update User set password_hash='"<<passwordHash<<"', "<<
			"salt='"<<salt<<"' where id =1";

	if(mysql_query(&mysql,query.str().c_str())){
		std::cout<<"Bład podczas aktualizacji haseł bazy danych!"<<std::endl;
	}
}

bool TicketDao::authenticateUser(std::string ip, std::string password)
{
	MYSQL_RES* queryId;	
	std::stringstream query;
	query << "select password_hash, salt from User u where u.ip='" <<ip<<"'";		
	mysql_select_db(&mysql,DATABASE_NAME.c_str());
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
	query << "select count(*) from User_Has_Service us " <<
						"left join User u on u.id = us.user_id " <<
						"left join Service s on s.id = us.service_id " <<
						"where u.ip='"<<ip<<"' "<<
						"and s.name='"<<serviceName<<"'";
						
	mysql_select_db(&mysql,DATABASE_NAME.c_str());
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
	query << "select count(*) from Ticket t " <<
						"left join User u on u.id = t.user_id " <<
						"left join Service s on s.id = t.service_id " <<
						"where u.ip='"<<ip<<"' "<<
						"and s.name='"<<serviceName<<"' "<<
						"and t.expiry_date > now()";
						
	mysql_select_db(&mysql,DATABASE_NAME.c_str());
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
	query << "select count(*) from Ticket t " <<
						"left join User u on u.id = t.user_id " <<
						"left join Service s on s.id = t.service_id " <<
						"where u.ip='"<<ticket->ip<<"' "<<
						"and s.name='"<<ticket->serviceName<<"' "<<
						"and t.checksum='"<<checksumString<<"' "<<
						"and t.expiry_date > now()";
						
	mysql_select_db(&mysql,DATABASE_NAME.c_str());
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
	mysql_select_db(&mysql,DATABASE_NAME.c_str());
	std::stringstream query;
	query << "update Ticket set expiry_date = from_unixtime("<<std::to_string(timestamp)<<"), checksum='"<<checksumString<<"' "<<
			" where user_id = (select id from User where ip = '"<<ip<<"') and service_id = (select id from Service where name = '"<<serviceName<<"')"; 
					
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
	mysql_select_db(&mysql,DATABASE_NAME.c_str());
	std::stringstream query;
	query << "insert into Ticket(user_id,service_id,checksum,expiry_date) "<<
			"select u.id, s.id,'"<<checksumString<<"',from_unixtime("<<std::to_string(timestamp)<<") "
			"from User u, Service s "<< 
			"where u.ip='"<<ip<<"' and s.name='"<<serviceName<<"'";
				
	if(mysql_query(&mysql,query.str().c_str())){
		return nullptr;
	}
	
	return ticket;
}
