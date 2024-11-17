#ifndef SERVER_H
#define SERVER_H

#include "server_client.h"

Client clients[MAX_CLIENTS];
int actual_clients;

PlayerInfo players[MAX_PLAYER_COUNT];
int actual_players = 0; // no database as of now

static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(Client *client, const char *buffer, char from_server);
static void remove_client(int to_remove);
static void clear_clients();
void manage_timeout(Client *client, unsigned int duration, State to_check, char *message,  void (*action)(Client *, char *));

int index_name_client(Client clients[MAX_CLIENTS], int currentCount, char name[MAX_NAME_SIZE]);
int index_name_player(int currentCount, char name[MAX_NAME_SIZE]);
Bool are_friend(PlayerInfo* player1, PlayerInfo* player2);
void add_friend(PlayerInfo* player, Response_Friend* response);
Bool accept_friend(Client *responder);

void read_request(Client* requester, const char* req);

void create_game(Client* client1, Client* client2);
Bool accept_challenge(Client* challenged);
Bool add_observer(Game* game, Client* observer);
Bool remove_observer(Client* observer);
void bitfieldToString(Joueur JCourant, BitField_1o cases, char* buffer);
void print_board_to(Joueur dest, Game* game, unsigned int ob_ind);
void continue_game(Game* game);
Bool make_move(Game* game, NumCase played_house);
Bool is_current_player(Game* game, Client* client);
void end_game(Game* game);
void cancel_game(Client* client, char* message);
void disconnect_players_from_game(Client * disconnected, char * message);

#endif /* guard */
