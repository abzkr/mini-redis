#ifndef PARSER_H
#define PARSER_H

#include <unordered_map>

std::string parser(char command_msg[4096], std::unordered_map<std::string , std::string> &kv_store);


#endif