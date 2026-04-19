#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>

/*
Checklist 
1) Understand how std;;strings work
2) "SET USER ABE" - User:ABE -> std::unordered_map kv_store 
3) "GET USER" -> Retrieve it from kv_store -> Returns ABE
4) "DEL USER" -> Delete abe
5) 
*/

int max_char;

int parser(std::string command_msg, std::unordered_map<std::string, int> kv_store) {
    
    std::stringstream command_stream (command_msg);

    std::string get = "GET";
    std::string set = "SET";
    std::string del = "DEL";

    return 0 ;
}