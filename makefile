all: server client

server: server.cc utils.cc cache.cc
	c++ -o server server.cc utils.cc cache.cc -std=c++11

client: client.cc utils.cc
	c++ -o client client.cc utils.cc

clean:
	rm -rf a.out* client server *.dat
