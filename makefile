CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lboost_locale -lboost_thread -lpthread

all: tftpClient
	g++ -o bin/echoExample bin/connectionHandler.o bin/tftpClient.o $(LDFLAGS)

tftpClient: bin/connectionHandler.o bin/tftpClient.o
	
bin/connectionHandler.o: src/connectionHandler.cpp
	g++ $(CFLAGS) -o bin/connectionHandler.o src/connectionHandler.cpp

bin/tftpClient.o: src/tftpClient.cpp
	g++ $(CFLAGS) -o bin/tftpClient.o src/tftpClient.cpp
	
.PHONY: clean
clean:
	rm -f bin/*
