CC = g++
CPPFLAGS = 
LDLIBS = 

TicketsServer: Main.o
	$(CC) -o TicketsClient Main.o $(CPPFLAGS) $(LDLIBS)
Main.o:
	$(CC) $(CPPFLAGS) -c Main.cpp
	
clean:
	rm -f *.o TicketsClient