CC = g++ -std=c++11
CPPFLAGS = 
LDLIBS = 

TicketsServer: Main.o TcpEcho.o UdpEcho.o UdpTime.o ReleaseTicket.o
	$(CC) -o TicketsClient Main.o TcpEcho.o UdpEcho.o UdpTime.o ReleaseTicket.o $(CPPFLAGS) $(LDLIBS)
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
	
clean:
	rm -f *.o TicketsClient
