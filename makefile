all: server client bench

server: server.cc utils.cc cache.cc
	c++ -o server server.cc utils.cc cache.cc -std=c++11

client: client.cc utils.cc
	c++ -o client client.cc utils.cc

bench: cache_bench.cc cache.cc
	c++ -o bench cache_bench.cc cache.cc -std=c++11

clean:
	rm -rf a.out* client server bench *.dat
