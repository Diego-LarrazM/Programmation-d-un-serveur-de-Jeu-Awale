#ifndef CLIENT_H
#define CLIENT_H


#include "../Libraries/stdvars.h"
#include "../Libraries/request.h"
#include "../Libraries/Awale/awale.h"

#define NON_OBSERVER (MAX_OBSERVERS + 1)

         
typedef enum {
LISTENING = 0, 
LOGGED_OUT = -2,
DISCONNECTED_FGAME = -1, // only if force-disconnected from game
IDLE = 1, 
CHALLENGING = 2, 
AWAITING_CHALLENGE = 3, 
IN_GAME = 4,
RESPONDING_CHALLENGE = 5,
OBSERVING = 6
} State;

struct struct_Client;
struct struct_Player;
struct struct_Game;

typedef struct struct_Player
{
   char bio[MAX_BIO_SIZE];
   char name[MAX_NAME_SIZE];
   char password[MAX_PASSWORD_SIZE];

   State player_state;
   unsigned int observer_index;

   PlayerRequestInfo friend_requests[MAX_FRIEND_REQUEST_SIZE];
   unsigned int friend_request_count;
   struct struct_Player* friends[MAX_PLAYER_COUNT];
   unsigned int friend_count;

   struct struct_Client* client;
   struct struct_Game* current_game;

}PlayerInfo;

typedef struct struct_Client
{
   SOCKET sock;
   PlayerInfo* player;
}Client;

typedef struct struct_Game
{
   Bool private;
   Plateau* game_board;
   Client* clients_involved[2];
   Client* observers[MAX_OBSERVERS];
   unsigned int nb_observers;
}Game;


#endif /* guard */
