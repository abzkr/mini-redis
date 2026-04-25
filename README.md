# mini-redis

A mini in-memory key-value store database written in C++ inspired by Redis.  
Currently implements a basic TCP echo server and is being developed toward full database functionality (currently working on the parser).

## Features
- TCP echo server: accepts client connections and handles low-level socket communication  
- Single client support: handles one client connection at a time  


## Usage
You can connect using telnet:
```bash
telnet localhost 6379
```

Once parser is implemented, send commands like:
```bash
SET user abzkr 
GET user
```

## Limitations
- Parser not yet implemented 
- No concurrency (single client only)
- No persistence 
- No authentication or security features
- Limited command set


## Current Project Status
The project currently implements a basic TCP echo server capable of accepting client connections.  
Command parsing, concurrency and persistence are planned next.
