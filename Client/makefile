CC = g++ -std=c++11
CPPFLAGS = 
LDLIBS = 

TicketsServer: Main.o tcpecho.o
	$(CC) -o TicketsClient Main.o tcpecho.o $(CPPFLAGS) $(LDLIBS)
Main.o:
	$(CC) $(CPPFLAGS) -c Main.cpp

tcpecho.o:
	$(CC) $(CPPFLAGS) -c tcpecho.cpp
	
clean:
	rm -f *.o TicketsClient
