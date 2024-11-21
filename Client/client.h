#ifndef CLIENT_H
#define CLIENT_H

#include "../Libraries/request.h"

static void init(void);
static void end(void);
static void app(const char *address, const char *name, const char *password);
static int init_connection(const char *address);
static void end_connection(int sock);
static int read_server(SOCKET sock, char *buffer);
static void write_server(SOCKET sock, const char *buffer);
static void write_server_request(SOCKET sock, const ClientRequest *request);

ClientRequest* create_request(const char* buffer);
void delete_request(ClientRequest* request);

#endif /* guard */
