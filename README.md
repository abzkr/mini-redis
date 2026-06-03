# mini-redis

A mini in-memory key-value store database written in C++ inspired by Redis.  
Currently implements a key-value storage system with a parser that works with SET, GET and DEL commands via TCP.

## Features
- TCP echo server: accepts client connections and handles low-level socket communication
- Command parser: supports SET, GET and DEL.
- Single client support: handles one client connection at a time  


## Usage
You can connect using telnet:
```bash
telnet localhost 6379
```

Commands:
```bash
SET user abzkr 
GET user
DEL user
```

## Limitations
- No concurrency (single client only)
- No persistence 
- No authentication or security features
- Limited command set


## Current Project Status
The project currently implements a basic command parser that works via TCP and is capable of accepting single client connections.  
More commands, concurrency and persistence are planned next.
