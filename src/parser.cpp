#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>


std::string parser(char command_msg[4096], std::unordered_map<std::string , std::string> &kv_store) {


    std::string command;
    std::string key;
    std::string value;

    std::stringstream command_stream (command_msg);
    command_stream >> command;

    if (command == "SET") {
        command_stream >> key;
        command_stream >> value;
        kv_store.insert({key,value});
        return "OK\n";
    }
    else if (command == "GET") {
        command_stream >> key;
        auto it = kv_store.find(key);
        if(it != kv_store.end()) return it->second + '\n';
    } 
    else if (command == "DEL") {
        command_stream >> key;
        auto it = kv_store.find(key);
        if(it != kv_store.end()) kv_store.erase(key);
        return "OK\n" ;
    }
  
    return "ERROR'\n";
}


