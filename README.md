# mini-redis

A mini in-memory key-value store database written in C++ inspired by Redis.

Incorporates a simple parser that uses an `unordered_map` for key-value storage, non-blocking I/O and `epoll()` for concurrent network event handling.

## Features
- TCP server: accepts client connections and handles low-level socket communication
- Command parser: supports SET, GET and DEL.
- Multiple client support: uses Linux `epoll()` to construct an event loop that handles multiple clients concurrently. 


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
- Lack of data persistence  
- Limited command set
- Parser limited to basic text (i.e No type handling such as for arrays etc)
- No strict TCP handshake enforcement

## Current Project Status
The project currently implements a basic command parser that works via TCP and is capable of accepting and handling up to 10 client connections.
### To do 
- Implementing a parser protocol that offers wider type handling and storage.
- Bench marking `epoll()` against other concurrency models/techniques.


## Single-Threaded Concurrency: Epoll() 
Linux epoll() combined with non-blocking I/O offers us a high-performance event-driven way of handling concurrent events across group of different network endpoints.


