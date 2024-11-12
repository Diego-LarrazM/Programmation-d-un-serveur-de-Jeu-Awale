#ifndef SERVER_H
#define SERVER_H

#include "../network.h"
#include "client.h"
#include "awale.h"
#include "request.h"

static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(Client *clients, Client client, int actual, const char *buffer, char from_server);
static void remove_client(Client *clients, int to_remove, int *actual);
static void clear_clients(Client *clients, int actual);

int index_name_client(Client clients[MAX_CLIENTS], int currentCount, char name[MAX_NAME_SIZE]);
int index_name_player(PlayerInfo players[MAX_PLAYER_COUNT], int currentCount, char name[MAX_NAME_SIZE]);

#endif /* guard */
