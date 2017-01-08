CC = g++
CPPFLAGS = -I/usr/include/mysql
LDLIBS = -lmysqlclient -L/usr/include/mysql

TicketsServer: Main.o TicketDao.o
	$(CC) -o TicketsServer Main.o TicketDao.o $(CPPFLAGS) $(LDLIBS)
Main.o:
	$(CC) $(CPPFLAGS) -std=c++11 -c Main.cpp
TicketDao.o:
	$(CC) $(CPPFLAGS) -std=c++11 -c TicketDao.cpp
	
clean:
	rm -f *.o TicketsServer