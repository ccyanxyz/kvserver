all: server server_tp client bench bench_no_map

tp: server_tp client

bench: bench bench_no_map

server: server.cc utils.cc cache.cc
	c++ -g -o server server.cc utils.cc cache.cc -std=c++11

server_tp: server_tp.cc utils.cc cache.cc thread_pool.cc
	c++ -g -o server_tp server_tp.cc utils.cc cache.cc thread_pool.cc -std=c++11

client: client.cc utils.cc
	c++ -g -o client client.cc utils.cc

bench: cache_bench.cc cache.cc
	c++ -g -o bench cache_bench.cc cache.cc -std=c++11

bench_no_map: cache_bench.cc cache.cc
	c++ -g -o bench_no_map -DNO_MAP cache_bench.cc cache.cc -std=c++11

clean:
	rm -rf a.out* client server server_tp bench bench_no_map *.dat *.dSYM
