#include "stdvars.h"
#include "network.h"
/*
#define LOGOUT 0
#define PROFILE 1
#define MESSAGE 2
#define CHALLENGE 3
#define PLAY 4
#define MOVE 5
#define FRIEND 6
#define RESPOND 7
#define ACTIVE_PLAYERS 8
#define ACTIVE_GAMES 9
#define OBSERVE 10
#define QUIT 11
*/
typedef enum  {LOGOUT = 0, PROFILE = 1, MESSAGE = 2, CHALLENGE = 3, PLAY = 4, MOVE = 5, FRIEND = 6, RESPOND = 7, ACTIVE_PLAYERS = 8, ACTIVE_GAMES = 9, OBSERVE = 10, QUIT = 11} RequestSignature;
typedef enum  {FRIEND_RESPOND = 0, CHALLENGE_RESPONSE = 1} ResponseType;


typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   char data[BUF_SIZE]; // MAX BUF_SIZE admitted: 65535
}ClientRequest;
/*
<signatures>              <DATA>
Logout          0          -
Profile         1          0, player_name or 1 (ours), -
Message         2          0 (all)  or 1 player_name
Challenge       3          0 or 1(Private), player_name
Play            4          0 or 1(Private), 0 (Offline) or 1 (Online)
Move            5          house chosen
Friend          6          player_name
Respond         7          type, 0 (decline) or 1 (accept), [PlayerName]
Active_players  8          0 (all) 1 (friends only)
Active_games    9          0 (all) 1 (friends only)
Observe         10         gameName
Quit            11         exit state (game played or observed)
*/

typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   Bool is_me;
   char player_name[MAX_NAME_SIZE]; // MAX BUF_SIZE admitted: 65535 - 1
   //char excess[BUF_SIZE - 1 - MAX_NAME_SIZE]; 
}ProfileRequest;

typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   Bool player;
   char player_name[MAX_NAME_SIZE]; // MAX BUF_SIZE admitted: 65535 - 1
   char message[BUF_SIZE - sizeof(Bool) - MAX_NAME_SIZE];
}MessageRequest;

typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   Bool private;
   char player_name[MAX_NAME_SIZE]; // MAX BUF_SIZE admitted: 65535 - 1
   //char excess[BUF_SIZE - sizeof(Bool) - MAX_NAME_SIZE]; 
}ChallengeRequest;

typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   Bool private;
   Bool online;
   //char excess[BUF_SIZE - 2*sizeof(Bool)]; 
}PlayRequest;

typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   NumCase played_house;
   //char excess[BUF_SIZE - 1]; 
}MoveRequest;

typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   char player_name[MAX_NAME_SIZE]; // MAX BUF_SIZE admitted: 65535 - 1
   //char excess[BUF_SIZE - MAX_NAME_SIZE]; 
}FriendRequest;

//////////////////////////////////////////////////////////
/*
Response Types:
0: Friend response [validation,PlayerName]
1: Challenge response [validation]
*/


typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   ResponseType response_type;
   Bool validation;
   //char excess[BUF_SIZE - sizeof(Bool) - sizeof(ResponseType)]; 
}Response;

typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   ResponseType response_type;
   Bool validation;
   char player_name[MAX_NAME_SIZE];
   //char excess[BUF_SIZE - sizeof(Bool) - sizeof(ResponseType) - MAX_NAME_SIZE]; 
}Response_Friend;

typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   ResponseType response_type;
   Bool validation;
   //char excess[BUF_SIZE - sizeof(Bool) - sizeof(ResponseType)]; 
}Response_Challenge;

//////////////////////////////////////////////////////////
typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   Bool friends_only;
   //char excess[BUF_SIZE - sizeof(Bool)]; 
}SeeActivePlayersRequest;

typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   Bool friends_only;
   //char excess[BUF_SIZE - sizeof(Bool)]; 
}SeeActiveGamesRequest;

typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   char player_name[MAX_NAME_SIZE]; 
   //char excess[BUF_SIZE - MAX_NAME_SIZE]; 
}ObserveRequest;



/////////////////////////////////////////

typedef struct {
   RequestSignature signature: 16;
   unsigned int size: 16; 
   char player_name[MAX_NAME_SIZE]; 
   unsigned int TTL;
}PlayerRequestInfo;
