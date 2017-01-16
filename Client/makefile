CC = g++ -std=c++11
CPPFLAGS = 
LDLIBS = -lcrypto

TicketsServer: Main.o TcpEcho.o UdpEcho.o UdpTime.o ReleaseTicket.o Crypto.o DataStructures.o
	$(CC) -o TicketsClient Main.o TcpEcho.o UdpEcho.o UdpTime.o ReleaseTicket.o Crypto.o DataStructures.o $(CPPFLAGS) $(LDLIBS)
Main.o:
	$(CC) $(CPPFLAGS) -c Main.cpp

TcpEcho.o:
	$(CC) $(CPPFLAGS) -c TcpEcho.cpp

UdpEcho.o:
	$(CC) $(CPPFLAGS) -c UdpEcho.cpp

UdpTime.o:
	$(CC) $(CPPFLAGS) -c UdpTime.cpp

ReleaseTicket.o:
	$(CC) $(CPPFLAGS) -c ReleaseTicket.cpp
	
Crypto.o:
	$(CC) $(CPPFLAGS) -c Crypto.cpp
	
DataStructures.o:
	$(CC) $(CPPFLAGS) -c DataStructures.cpp
	
clean:
	rm -f *.o TicketsClient
