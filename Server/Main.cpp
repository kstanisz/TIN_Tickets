#include<iostream>

#include"TicketDao.h"

int main(){
	
	TicketDao* ticketDao = new TicketDao();
	
	bool connection = ticketDao->connect();
	if(!connection)
	{
		std::cout<<"Nie udało się nawiązać połączenia"<<std::endl;
		return 0;
	}
	
	bool access = ticketDao->checkAccess("127.0.0.0","password1","SERVICE1");
	if(access)
	{
		std::cout<<"Użytkownik ma dostęp do serwisu"<<std::endl;
	}
	else
	{
		std::cout<<"Użytkownik nie ma dostępu do serwisu"<<std::endl;
	}
	
	ticketDao->releaseTicket("127.0.0.0","password1","SERVICE1");
	
	bool isTicket = ticketDao->checkTicket("127.0.0.0","password1","SERVICE1");
	if(isTicket)
	{
		std::cout<<"Bilet aktualny"<<std::endl;
	}
	else
	{
		std::cout<<"Brak biletu"<<std::endl;
	}
	
	ticketDao->disconnect();
	
	return 0;
}