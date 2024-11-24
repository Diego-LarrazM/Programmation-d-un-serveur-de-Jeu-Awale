#ifndef CLIENT_H
#define CLIENT_H

#include "../Libraries/request.h"

static void init(void);
/*
Function to initiate the sockets (specially for windows users)
*/

static void end(void);
/*
Function to end the sockets (specially for windows users)
*/

static void app(const char *address, const char *name, const char *password);
/*
The heart of the client application that loop infinitly

Parameters:
const char *address : the address to the server to connect to
const char *name : the name the user will take in the server
const char *password : the password the user set
*/

static int init_connection(const char *address);
/*
Function that tries to connect to the server with the entered address and return its socket value

Parameter:
const char *address : the address to the server to connect to

Return:
int : the value of the socket
*/

static void end_connection(int sock);
/*
Function that closes the socket put in argument

Parameter:
int sock : the value of the socket to close
*/

static int read_server(SOCKET sock, char *buffer);
/*
Function that reads what the server has sent and put it in the buffer in argument

Parameters:
SOCKET sock : the socket to the server to listen to
char *buffer : the buffer where to write what the server is sending
*/

static void write_server(SOCKET sock, const char *buffer);
/*
Function that write to the server what's inside of the buffer put in argument
Used to send a string format message

Parameters:
SOCKET sock : the socket to the server to write to
char *buffer : contains what to write to the server
*/

static void write_server_request(SOCKET sock, const ClientRequest *request);
/*
Function that write to the server the request put in argument
Used to send a request struct format message

Parameters:
SOCKET sock : the socket to the server to write to
const ClientRequest *request : contains what to write to the server
*/

ClientRequest* create_request(const char* buffer);
/*
Function that create the request the user wants to send to the user with the user's command

Parameter:
const char* buffer : the buffer containing the user command

Return:
ClientRequest* : the request formed from the command inputed (its is 0 if an uncorrect command has been inputed)
*/

void delete_request(ClientRequest* request);
/*
Function that deletes the request put in argument

Parameter:
ClientRequest* request : the request to delete
*/

#endif /* guard */
