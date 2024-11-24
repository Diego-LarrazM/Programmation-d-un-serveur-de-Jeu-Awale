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
/*
Function to initiate the sockets (specially for windows users)
*/

static void end(void);
/*
Function to end the sockets (specially for windows users)
*/

static void app(void);
/*
The heart of the server application that loop infinitly
*/

static int init_connection(void);
/*
Function that creates a socket for the server

Return:
int: the value of the socket
*/

static void end_connection(int sock);
/*
Function that closes the socket put in argument

Parameter:
int sock: the value of the socket to close
*/

static int read_client(SOCKET sock, char *buffer);
/*
Function that reads what the client has sent and put it in the buffer in argument

Parameters:
SOCKET sock: the socket of the client that is sending something
char *buffer: the buffer where to write what the client is sending
*/

static void write_client(SOCKET sock, const char *buffer);
/*
Function that writes to the client what's inside of the buffer put in argument

Parameters:
SOCKET sock: the socket of the client to write to
const char *buffer: contains what to write to the client
*/

static void send_message_to_all_clients(Client *sender, const char *buffer, Bool from_server);
/*
Function that sends to all clients what's inside of the buffer put in argument

Parameters:
Client *sender: the socket of the client to not send a message to
const char *buffer: contains what to write to every client
Bool from_server: indicate if the message originate from the server
                  (meaning if the name of the client sender should be put in the message)
*/

static void send_error_message(Client* client, const char *message, const State default_state);
/*
Function that sends to the client put in argument a message as well as to change his state to the one in argument

Parameters:
Client *client: the socket of the client to send the message to
const char *message: contains what to write to the client
 const State default_state: the state of the player to change to after writing the message
*/

static void remove_client(int to_remove);
/*
Function that deletes a client in the array of clients

Parameter:
int to_remove: index of the client to remove inside of the array
*/

static void clear_clients();
/*
Function that deletes all clients in the array of clients and close their socket
*/

static void clear_players();
/*
Function that deletes all players in the array of players
*/

void manage_timeout(PlayerInfo *player, const State to_check, const char *message, void* (*action)(void *));
/*
Function that manages the timeout system

Parameters:
PlayerInfo *player: the player that initiated the timeout
const State to_check: if the player's state is equal to the one in argument after the timeout waiting time
                      then the timeout effet applies
const char *message: the message to send to the player if the timeout effet applies
void* (*action)(void *): the function to call for the timeout thread
*/

void set_initial_player(PlayerInfo* player);
/*
Function that sets the initial values of the PlayerInfo struct

Parameter:
PlayerInfo* player: the player's PlayerInfo struct to set its initial values

*/

int index_name_client(const char name[MAX_NAME_SIZE]);
/*
Function that returns the value of the index of the client in the array of clients by its name

Parameter:
const char name[MAX_NAME_SIZE]: the client's name to search

Return:
int: the index of the client in the array of clients (equal to actual_clients if didn't find)
*/

int index_name_player(const char name[MAX_NAME_SIZE]);
/*
Function that returns the value of the index of the player in the array of players by its name

Parameter:
const char name[MAX_NAME_SIZE]: the player's name to search

Return:
int: the index of the player in the array of players (equal to actual_players if didn't find)
*/

Bool are_friend(const PlayerInfo* player1, const PlayerInfo* player2);
/*
Function that tests if two players are friends

Parameters:
const PlayerInfo* player1: the first player
const PlayerInfo* player2: the second player

Return:
Bool: if both player are friends
*/

void add_friend(PlayerInfo* player1, PlayerInfo* player2);
/*
Function that adds the other player to their friend list
/!\ this function doesn't test if they are already friends

Parameters:
PlayerInfo* player1: the first player
PlayerInfo* player2: the second player
*/

void decline_friend(PlayerInfo* declined, const char* message);
/*
Function that declines a player's befriend request
/!\ this function doesn't check if the player has received a friend request

Parameters:
PlayerInfo* declined: the player that decline the friend request
const char* message: the message to send to the other player after the befriend as been declined
*/

void read_request(Client *requester, const char *req);
/*
Function that treats the request that the player has sent

Parameters:
Client *requester: client of the player that has sent the request
const char *req: pointer to the buffer containing the request
*/

void create_game(Client *client1, Client *client2, Bool private);
/*
Function that creates a game between the two player's client entered in argument

Parameters:
Client *client1: the client of the first player
Client *client2: the client of the second player
Bool private: if the game is private (to manage observers & game list)
*/

void accept_challenge(Client* challenged);
/*
Function that accepts the challenge that a player has received
/!\ this function doesn't check if the player has received a challenge

Parameter:
Client* challenged: the client of the player that has accepted the challenge
*/

Bool add_observer(Game* game, Client* observer);
/*
Function that adds an observer to the game
(if the game is set to private, only friends can be observers)

Parameters:
Game* game: the game to add the observer to
Client* observer: the player's client to add as an observer

Return:
Bool: if the player has been added to the list of observers of the game
*/

void remove_observer(Game *game, unsigned int o_index);
/*
Function that removes an observer of the array of observer of the game

Parameters:
Game* game: the game to remove the observer from
unsigned int o_index: the index of the observer to remove
*/

void quit_observing_game(Client *observer);
/*
Function that removes the client from observing a game
/!\ this function doesn't check if the client is observing a game

Parameter:
Client *observer: the player's client to quit observing
*/

void bitfieldToString(Joueur JCourant, BitField_1o cases, char* buffer);
/*
Function that translates a bit field into a series of numbers (playables houses) depending of the player

Parameters:
Joueur JCourant: the player to take his POV to get the series of numbers
BitField_1o cases: the bit field to transform to text
char* buffer: the buffer to print the text to
*/

void print_board_to(Joueur dest, const Game* game, unsigned int ob_ind);
/*
Function that prints the board on the client side for either player1 or player2 or to an observer of the game.

Parameters:
Joueur dest: who to send the board state to
const Game* game: the game to take the board state from
unsigned int ob_ind: the index of the observer to send the board state to
*/

void continue_game(Game* game);
/*
Function that ends and start a new turn of the game

Parameter:
Game* game: the game
*/

Bool make_move(Game* game, NumCase played_house);
/*
Function that plays the move the current player's turn wants to, and print the move to players and observers

Parameters:
Game* game: the game
NumCase played_house: the house number the current player's turn wants to play

Return:
Bool: if the move was possible and has been played
*/

Bool is_current_player(const Game* game, const Client* client);
/*
Function that checks if the client put in argument is the current player's turn of the game

Parameters:
const Game* game: the game
const Client* client: the player's client to check if he's the current's turn player

Return:
Bool: if that player is the current player's turn to play
*/

void end_game(Game* game);
/*
Function that deletes and end the game

Parameter:
const Game* game: the game to delete & end
*/

void* decline_friend_timeout(void* arg);
/*
Function that manages the friend request timeout (called by manage_timeout)

Parameter:
void* arg: pointer to the Timeout struct with information on the timeout
*/

void* disconnect_players_from_game(void* arg);
/*
Function that manages the disconnect from game timeout (called by manage_timeout)

Parameter:
void* arg: pointer to the Timeout struct with information on the timeout
*/

void set_bio_player(PlayerInfo* player, char* bio);
/*
Function that sets the player's bio to the new bio put in argument

Parameters:
PlayerInfo* player: the player to change his bio
char* bio: the new to bio of the player
*/
void write_profile(Client* client, PlayerInfo* player);
/*
Function that writes to the client a player's profile (composed of the player's name, games played, games won and his bio)

Parameters:
Client* client: the client to send the player's profile
PlayerInfo* player: the player's profile to show
*/

#endif /* guard */
