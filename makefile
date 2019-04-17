all: server client bench bench_no_map

cs: server client

bench: bench bench_no_map

server: server.cc utils.cc cache.cc thread_pool.cc
	c++ -g -o server server.cc utils.cc cache.cc thread_pool.cc -std=c++11

client: client.cc utils.cc
	c++ -g -o client client.cc utils.cc

bench: cache_bench.cc cache.cc
	c++ -g -o bench cache_bench.cc cache.cc -std=c++11

bench_no_map: cache_bench.cc cache.cc
	c++ -g -o bench_no_map -DNO_MAP cache_bench.cc cache.cc -std=c++11

clean:
	rm -rf a.out* client server bench bench_no_map *.dat *.dSYM
