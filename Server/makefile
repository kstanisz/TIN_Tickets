CC = g++
CPPFLAGS = -I/usr/include/mysql
LDLIBS = -lmysqlclient -lcrypto -L/usr/include/mysql 
 
TicketsServer: Main.o Crypto.o TicketDao.o DataStructures.o
	$(CC) -o TicketsServer Main.o Crypto.o TicketDao.o DataStructures.o $(CPPFLAGS) $(LDLIBS)
Main.o:
	$(CC) $(CPPFLAGS) -std=c++11 -c Main.cpp
Crypto.o:
	$(CC) $(CPPFLAGS) -std=c++11 -c Crypto.cpp
TicketDao.o:
	$(CC) $(CPPFLAGS) -std=c++11 -c TicketDao.cpp
DataStructures.o:
	$(CC) $(CPPFLAGS) -std=c++11 -c DataStructures.cpp
	
clean:
	rm -f *.o TicketsServer
