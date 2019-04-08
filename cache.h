#ifndef _CACHE_H_
#define _CACHE_H_

#include <vector>
#include <unordered_map>
#include <fstream>
#include <string>

class Cache {
private:
	const static int PAGE_SIZE = 2;
	int capacity;

	class Page;
	class Node;
	Page *entries;
	std::vector<Page *> free_entries;

	std::unordered_map<std::string, Node *> table;
	std::unordered_map<int, bool> file_list;
	Page *current_page;
	bool current_page_flag;
	int data_index;

	int file_index;
	Page *head, *tail;

	Page *get_new_page();

	void detach(Page *page);
	void attach(Page *page);

	Node *search(std::string key);

	bool load_file_to_page(const int &file_num, Page *page, std::unordered_map<std::string, Node *> &table);
	bool save_page_to_file(Page *page);
	void erase_page(Page *page, std::unordered_map<std::string, Node *> &table);

	int load_file_index(const std::string &filename = "file_index.dat");
	bool save_file_index(const std::string &filename = "file_index.dat");

public:
	Cache(int capacity_ = 2);
	~Cache();
	void save_cache();
	std::string get(std::string key);
	void put(std::string key, std::string value);
};

#endif
