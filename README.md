# client_serverapp
 * Purpose: This program demonstrates a multithread client-server application that can handle asynchronous communication between the server and client.  The sever acts as a shell that can handle various commands from the client and responds to them appropriately.

 * The IPC mechanism chosen to accomplish this is the use of Pipes.  A reliable way to communicate, while handling large data.

 * * Files: server.c, client.c, makefile
 * -Compile using makefile
 * -OR
 * -Compile: 	gcc -pthread -Wall server.c -o server
		gcc -pthread -Wall client.c -o client

 * -Execute server first: ./server 
 * -Execute client(s): ./client
