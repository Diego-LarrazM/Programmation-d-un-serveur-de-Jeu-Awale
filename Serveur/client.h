#ifndef CLIENT_H
#define CLIENT_H

#include "server.h"

#define MAX_NAME_SIZE 50
#define MAX_PASSWORD_SIZE 50
#define MAX_BIO_SIZE 50
#define MAX_PLAYER_COUNT 1024

typedef enum {
AWAITING_RESPONSE = 0, 
DISCONECTED_FGAME = -1, // only if disconexion from game
IDLE = 1, 
CHALLENGING = 2, 
IN_GAME = 3,
RESPONDING = 4
} State;

typedef struct
{
   char bio[MAX_BIO_SIZE];
   char name[MAX_NAME_SIZE];
   char password[MAX_PASSWORD_SIZE];
   State playerState;
}PlayerInfo;

typedef struct
{
   SOCKET sock;
   PlayerInfo* player;
}Client;

#endif /* guard */
