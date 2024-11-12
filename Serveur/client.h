#ifndef CLIENT_H
#define CLIENT_H

#include "server.h"
#include "../network.h"
#include "request.h"

#define MAX_NAME_SIZE 50
#define MAX_PASSWORD_SIZE 50
#define MAX_BIO_SIZE 50
#define MAX_PLAYER_COUNT 1024
#define MAX_REQUEST_SIZE 1024

typedef enum {
LISTENING = 0, 
LOGGED_OUT = -2,
DISCONNECTED_FGAME = -1, // only if force-disconnected from game
IDLE = 1, 
CHALLENGING = 2, 
AWAITING_CHALLENGE = 3, 
IN_GAME = 4,
RESPONDING = 5,
OBSERVING = 6
} State;

struct struct_Client;

typedef struct
{
   char bio[MAX_BIO_SIZE];
   char name[MAX_NAME_SIZE];
   char password[MAX_PASSWORD_SIZE];

   State player_state;

   PlayerRequestInfo requests[100];  // Linked list ?
   PlayerInfo* friends[MAX_PLAYER_COUNT]; // Linked list ?
   unsigned int friend_count;

   struct struct_Client* client;
}PlayerInfo;

typedef struct struct_Client
{
   SOCKET sock;
   PlayerInfo* player;
}Client;

#endif /* guard */
