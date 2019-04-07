#ifndef _UTILS_H_
#define _UTILS_H_

#include "node.h"
#include <vector>

Node *str_to_node(const std::string &str);
std::string node_to_str(const Node *node);

std::vector<std::string> split(std::string src, const std::string &delim);

#endif
