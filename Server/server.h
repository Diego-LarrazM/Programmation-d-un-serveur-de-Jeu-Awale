#ifndef SERVER_H
#define SERVER_H

#include "server_client.h"

static Client* clients[MAX_CLIENTS];
static int actual_clients;

static PlayerInfo* players[MAX_PLAYER_COUNT];
static int actual_players = 0; // no database as of now

typedef struct TimeOut_struct{
    PlayerInfo *player;
    unsigned int duration;
    State to_check;
    char message[BUF_SIZE];
    pthread_t thread;
} Timeout;



static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(Client *client, const char *buffer, Bool from_server);
static void send_error_message(Client* client, const char *message, const State default_state);
static void remove_client(int to_remove);
static void clear_clients();
static void clear_players();
void manage_timeout(PlayerInfo *player, const State to_check, const char *message, void* (*action)(void *));
void set_initial_player(PlayerInfo* player);

int index_name_client(const char name[MAX_NAME_SIZE]);
int index_name_player(const char name[MAX_NAME_SIZE]);
Bool are_friend(const PlayerInfo* player1, const PlayerInfo* player2);
void add_friend(PlayerInfo* player1, PlayerInfo* player2);
void decline_friend(PlayerInfo* declined, const char* message);

void read_request(Client *requester, const char *req);

void create_game(Client *client1, Client *client2, Bool private);
Bool accept_challenge(Client* challenged);
Bool add_observer(Game* game, Client* observer);
void remove_observer(Game *game, unsigned int o_index);
void quit_observing_game(Client *observer);
void bitfieldToString(Joueur JCourant, BitField_1o cases, char* buffer);
void print_board_to(Joueur dest, const Game* game, unsigned int ob_ind);
void continue_game(Game* game);
Bool make_move(Game* game, NumCase played_house);
Bool is_current_player(const Game* game, const Client* client);
void end_game(Game* game);

void* decline_friend_timeout(void* arg);
void* disconnect_players_from_game(void* arg);

void set_bio_player(PlayerInfo* player, char* bio);
void write_profile(Client* client, PlayerInfo* player);

#endif /* guard */
