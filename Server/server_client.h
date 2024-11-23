#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H


#include "../Libraries/Awale/awale.h"
#include "../Libraries/request.h"
//#include "../Libraries/stdvars.h"
//#include "../Libraries/network.h"
#include <pthread.h>

#define NON_OBSERVER (MAX_OBSERVERS + 1)

         
typedef enum {
UNDEFINED = -3,
LISTENING = 0, 
LOGGED_OUT = -2,
DISCONNECTED_FGAME = -1, // only if force-disconnected from game
IDLE = 1, 
CHALLENGING = 2, 
AWAITING_CHALLENGE = 3, 
IN_GAME = 4,
RESPONDING_CHALLENGE = 5,
OBSERVING = 6,
AWAITING_FRIEND = 7, 
RESPONDING_FRIEND = 8
} State;

struct struct_client;
struct struct_player;
struct struct_game;

typedef struct struct_player
{
   char bio[MAX_BIO_SIZE];
   char name[MAX_NAME_SIZE];
   char password[MAX_PASSWORD_SIZE];

   State player_state;
   unsigned int observer_index;

   struct struct_player* friends[MAX_PLAYER_COUNT];
   unsigned int friend_count;
   struct struct_player* asking_friends;

   struct struct_client* client;
   struct struct_game* current_game;

   unsigned int nb_games;
   unsigned int nb_wins;

}PlayerInfo;

typedef struct struct_client
{
   SOCKET sock;
   PlayerInfo* player;
}Client;

typedef struct struct_game
{
   Bool private;
   Plateau* game_board;
   PlayerInfo* players_involved[2];
   Client* observers[MAX_OBSERVERS];
   unsigned int nb_observers;
}Game;

#endif /* guard */
