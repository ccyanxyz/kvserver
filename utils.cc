#include "utils.h"

std::vector<std::string> split(std::string str, const std::string &delim)
{
	std::vector<std::string> vec;
	str += delim[0];
	std::string::size_type start = 0;
	std::string::size_type index = str.find_first_of(delim, start);
	while(index != std::string::npos) {
		vec.push_back(str.substr(start, index - start));
		start = str.find_first_not_of(delim, index);
		if(start == std::string::npos) {
			break;
		}
		index = str.find_first_of(delim, start);
	}
	return vec;
}

Node *str_to_node(const std::string &str)
{
	std::vector<std::string> vec = split(str, "-;:\n");
	Node *node = new Node;
	if(vec.size() == 0) {
		return NULL;
	} else if(vec.size() == 1) {
		node->key = vec[0];
		node->value = "FAILED";
	} else {
		node->key = vec[0];
		node->value = vec[1];
	}
	return node;
}

std::string node_to_str(const Node *node)
{
	std::string str;
	if(node == NULL) {
		return str;
	}
	str += node->key;
	str += "-";
	str += node->value;
	return str;
}
