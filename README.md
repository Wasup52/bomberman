# Networked Bomberman

Welcome to the GitHub repository for our student project - a networked version of the classic game Bomberman, written in C for Linux. This project was created as part of our computer science course and serves as a demonstration of our understanding of network programming and game development.

## Getting Started

To launch the game, you will need to first compile both the `bomberman-server.c` and `bomberman-client.c` files. This can be done by running the following command in the terminal:

```
gcc bomberman-server.c -o bomberman-server
gcc bomberman-client.c -o bomberman-client
```

Once the files have been compiled, the server must be launched first and enter the desired range of the bombs (n).

```
┌──(wasup㉿bash)-[~/…/bomberman]
└─$ ./bomberman-server
The socket is identified by : 3 
n: 2
waiting for client...
```

After the server has been launched, you can do the same thing with the client.  

```
┌──(wasup㉿bash)-[~/…/bomberman]
└─$ ./bomberman-client
The socket is identified by : 3 
n: 
```
Once both the server and client have been launched and the bomb range has been set, the game can be played.
