#include "cache.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <ctime>

using std::cerr;
using std::cout;
using std::endl;

std::string random_str(int len);
int random_int(const int upper_bound);
std::vector<std::pair<std::string, std::string> > generate_data(const int count, const int str_len);
void cache_bench_write(const int count, const int str_len);
void cache_bench_read(const int count);

Cache cache(50);
std::default_random_engine rand_engine(time(NULL));

int main(int argc, char *argv[])
{
	if(argc < 2) {
		cerr << "Usage: ./cache_bench <data_count>" << endl;
		exit(0);
	}

	int count = atoi(argv[1]);

	cache_bench_write(count, 4);
	cache_bench_read(count);

	return 0;
}

std::string random_str(int len)
{
	const static char charmap[62] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 
		'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
		'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
		'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
		'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z',
	};

	// [0, 61]
	std::uniform_int_distribution<int> dist(0, 61);

	std::string str;
	for(int i = 0; i < len; ++i) {
		str += charmap[dist(rand_engine)];
	}
	return str;
}

int random_int(const int upper_bound)
{
	std::uniform_int_distribution<int> dist(0, upper_bound - 1);
	return dist(rand_engine);
}

std::vector<std::pair<std::string, std::string> > generate_data(const int count, const int str_len)
{
	std::vector<std::pair<std::string, std::string> > vec;
	for(int i = 0; i < count; ++i) {
		vec.emplace_back(random_str(str_len), random_str(str_len));
	}
	return vec;
}

void cache_bench_write(const int count, const int str_len)
{
	std::vector<std::pair<std::string, std::string> > vec = generate_data(count, str_len);

	auto start = std::chrono::system_clock::now();
	for(auto it = vec.begin(); it != vec.end(); ++it) {
		cache.put(it->first, it->second);
	}
	auto end = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	double sec = double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	double avg_speed = count / sec; 

	cout << "WRITE:" << count << " key-value pairs, string length " << str_len << endl;
	cout << "\ttime used:" << sec << " seconds"<< endl;
	cout << "\taverage speed:" << avg_speed << " records/sec" << endl;
}

void cache_bench_read(const int count)
{
	std::vector<std::string> keys = cache.get_keys();

	auto start = std::chrono::system_clock::now();
	for(int i = 0; i < count; ++i) {
		cache.get(keys[random_int(keys.size())]);
	}
	auto end = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	double sec = double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	double avg_speed = count / sec; 

	cout << "RANDOM READ:" << count << " times" << endl;
	cout << "\ttime used:" << sec << " seconds"<< endl;
	cout << "\taverage speed:" << avg_speed << " records/sec" << endl;
}
