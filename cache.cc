#include "cache.h"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;

class Cache::Node {
public:
	std::string key;
	std::string value;
	class Page *page;
};

class Cache::Page {
public:
	int file_num;
	int data_index;
	bool lock;
	bool dirty;
	class Node data[PAGE_SIZE];
	class Page *prev, *next;
	Page()
	{
		file_num = -1;
		data_index = 0;
		lock = false;
		dirty = false;
		prev = next = NULL;
	}
};

Cache::Cache(int capacity_)
{
	capacity = capacity_;
	file_index = load_file_index();
	current_page_flag = false;

	entries = new Page[capacity];
	for(int i = 0; i < capacity; ++i) {
		for(int j = 0; j < PAGE_SIZE; ++j) {
			entries[i].data[j].page = &entries[i];
		}
		free_entries.push_back(&entries[i]);
	}

	head = new Page;
	tail = new Page;
	head->prev = NULL;
	head->next = tail;
	tail->prev = head;
	tail->next = NULL;
}

Cache::~Cache()
{
	save_cache();
	delete [] entries;
	delete head;
	delete tail;
}

Cache::Node *Cache::search(std::string key)
{
	Node *node = table[key];
	Page *page = NULL;
	if(node) {
		page = node->page;
		detach(page);
		attach(page);
	} else {
		for(int i = file_index; i >= 0; --i) {
			if(file_list[i] == true) {
				continue;
			}

			page = get_new_page();
			if(load_file_to_page(i, page, table) == false) {
				free_entries.push_back(page);
				continue;
			}

			if((node = table[key]) != NULL) {
				attach(page);
				break;
			} else {
				erase_page(page, table);
				free_entries.push_back(page);
			}
		}
	}
	return node;
}

std::string Cache::get(std::string key)
{
	Node *node = search(key);
	if(node) {
		return node->value;
	}
	return std::string();
}

void Cache::put(std::string key, std::string value)
{
	Node *node = search(key);
	if(node) {
		if(value != node->value) {
			node->value = value;
			node->page->dirty = true;
		}
	} else {
		if(data_index == PAGE_SIZE || current_page_flag == false) {
			data_index = 0;
		}
		if(data_index == 0 || current_page_flag == false) {
			current_page = get_new_page();
			current_page_flag = true;
			current_page->file_num = ++file_index;
			current_page->dirty = true;
			current_page->data_index = 0;
			for(int i = 0; i < PAGE_SIZE; ++i) {
				current_page->data[i].key = "";
				current_page->data[i].value = "";
				current_page->data[i].page = current_page;
			}
			attach(current_page);
		}
		current_page->data[data_index].key = key;
		current_page->data[data_index].value = value;
		table[key] = current_page->data + data_index;
		++data_index;
		current_page->data_index += 1;
		detach(current_page);
		attach(current_page);
	}
}

bool Cache::load_file_to_page(const int &file_num, Page *page, std::unordered_map<std::string, Node *> &table)
{
	std::string filename = std::to_string(file_num) + ".dat";
	ifstream in(filename);
	if(!in || page == NULL) {
		return false;
	}
	// file in table
	file_list[file_num] = true;
	page->file_num = file_num;
	Node *data = page->data;
	for(int i = 0; i < PAGE_SIZE; ++i) {
		if(!(in >> data[i].key >> data[i].value)) {
			data[i].key = "";
			data[i].value = "";
			data[i].page = page;
		} else {
			page->data_index += 1;
		}
		table[data[i].key] = data + i;
	}
	in.close();
	return true;
}

bool Cache::save_page_to_file(Page *page)
{
	page->dirty = false;
	if(page == NULL) {
		return false;
	}
	std::string filename = std::to_string(page->file_num) + ".dat";
	ofstream out(filename);
	Node *data = page->data;
	for(int i = 0; i < page->data_index; ++i) {
		out << data[i].key << " " << data[i].value << endl;
	}
	out.close();
	return true;
}

int Cache::load_file_index(const std::string &filename)
{
	ifstream in(filename);
	if(!in) {
		return false;
	}
	int index;
	in >> index;
	in.close();
	return index;
}

bool Cache::save_file_index(const std::string &filename)
{
	ofstream out(filename);
	if(!(out << file_index)) {
		cerr << "save file index " << file_index << " failed" << endl;
		return false;
	}
	out.close();
	return true;
}

void Cache::erase_page(Page *page, std::unordered_map<std::string, Node *> &table)
{
	file_list.erase(page->file_num);
	for(int i = 0; i < PAGE_SIZE; ++i) {
		table.erase(page->data[i].key);
	}
}

void Cache::save_cache()
{
	Page *page = head->next;
	while(page != tail) {
		if(page->dirty) {
			save_page_to_file(page);
		}
		page = page->next;
	}
	save_file_index();
}

void Cache::detach(Page *page)
{
	page->prev->next = page->next;
	page->next->prev = page->prev;
}

void Cache::attach(Page *page)
{
	page->prev = head;
	page->next = head->next;
	head->next->prev = page;
	head->next = page;
}

Cache::Page *Cache::get_new_page()
{
	Page *page;
	// search for page not full yet
	page = head->next;
	while(page != tail) {
		if(page->data_index < PAGE_SIZE) {
			break;
		}
		page = page->next;
	}
	
	if(page != tail) {
		return page;
	}

	if(free_entries.empty()) {
		// cache full, swap tail page out
		page = tail->prev;
		if(page == current_page) {
			current_page_flag = false;
		}
		if(page->dirty) {
			save_page_to_file(page);
		}
		erase_page(page, table);
		detach(page);
		free_entries.push_back(page);
	}
	page = free_entries.back();
	free_entries.pop_back();
	return page;
}
