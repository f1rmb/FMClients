CC      = cc
CXX     = c++
CFLAGS  = -g -O3 -Wall -std=c++0x -pthread
LIBS    = -lpthread
LDFLAGS = -g

OBJECTS = FMFileReader.o FMNetwork.o StopWatch.o Thread.o Timer.o UDPSocket.o WAVFileReader.o

all:		FMFileReader

FMFileReader:	$(OBJECTS)
		$(CXX) $(OBJECTS) $(CFLAGS) $(LIBS) -o FMFileReader

%.o: %.cpp
		$(CXX) $(CFLAGS) -c -o $@ $<

install:
		install -m 755 FMFileReader /usr/local/bin/

clean:
		$(RM) FMFileReader *.o *.d *.bak *~
