#ifndef SERVER_H
#define SERVER_H

#include "client.h"
#include "../Awale/awale.h"

static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(Client *clients, Client *client, int actual, const char *buffer, char from_server);
static void remove_client(Client *clients, int to_remove, int *actual);
static void clear_clients(Client *clients, int actual);

int index_name_client(Client clients[MAX_CLIENTS], int currentCount, char name[MAX_NAME_SIZE]);
int index_name_player(PlayerInfo players[MAX_PLAYER_COUNT], int currentCount, char name[MAX_NAME_SIZE]);
Bool are_friend(PlayerInfo* player1, PlayerInfo* player2);

void read_request(Client* clients, Client* requester, int actual_clients, const char* req);

void tick_TLLs(PlayerInfo* player);

void create_game(Client* client1, Client* client2);
Bool accept_challenge(Client* challenged);
void bitfieldToString(Joueur JCourant, BitField_1o cases, char* buffer);
void continue_game(Game* game);
Bool make_move(Game* game, NumCase played_house);
Bool is_current_player(Game* game, Client* client);
void end_game(Game* game);
void cancel_game(Client* client, char* message);

#endif /* guard */
