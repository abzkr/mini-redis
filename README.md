# mini-redis

A mini in-memory key-value store database written in C++ inspired by Redis.  
Currently implements a key-value storage system with a parser that works with SET, GET and DEL commands via TCP.

## Features
- TCP server: accepts client connections and handles low-level socket communication
- Command parser: supports SET, GET and DEL.
- Multiple client support: uses an epoll() instance and event loop to handle up to 10 client connections at a time  


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
- No data persistence 
- No authentication or security features
- Limited command set
- No performance benchmarking


## Current Project Status
The project currently implements a basic command parser that works via TCP and is capable of accepting and handling up to 10 client connections.
RESP implementation and persistence are planned next.
