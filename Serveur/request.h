typedef struct {
   unsigned int signature: 16;
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
Respond         7          0 (decline) or 1 (accept)
Active_players  8          0 (all) 1 (friends only)
Active_games    9          0 (all) 1 (friends only)
Observe         10         gameName
Quit            11         exit state (game played or observed)
*/

typedef struct {
   unsigned int signature: 16;
   unsigned int size: 16; 
   Bool is_me;
   char player_name[MAX_NAME_SIZE]; // MAX BUF_SIZE admitted: 65535 - 1
   //char excess[BUF_SIZE - 1 - MAX_NAME_SIZE]; 
}ProfileRequest;

typedef struct {
   unsigned int signature: 16;
   unsigned int size: 16; 
   Bool player;
   char player_name[MAX_NAME_SIZE]; // MAX BUF_SIZE admitted: 65535 - 1
   //char excess[BUF_SIZE - 1 - MAX_NAME_SIZE]; 
}MessageRequest;

typedef struct {
   unsigned int signature: 16;
   unsigned int size: 16; 
   Bool private;
   char player_name[MAX_NAME_SIZE]; // MAX BUF_SIZE admitted: 65535 - 1
   //char excess[BUF_SIZE - 1 - MAX_NAME_SIZE]; 
}ChallengeRequest;

typedef struct {
   unsigned int signature: 16;
   unsigned int size: 16; 
   Bool private;
   Bool online;
   //char excess[BUF_SIZE - 2]; 
}PlayRequest;

typedef struct {
   unsigned int signature: 16;
   unsigned int size: 16; 
   NumCase house;
   //char excess[BUF_SIZE - 1]; 
}MoveRequest;

typedef struct {
   unsigned int signature: 16;
   unsigned int size: 16; 
   char player_name[MAX_NAME_SIZE]; // MAX BUF_SIZE admitted: 65535 - 1
   //char excess[BUF_SIZE - MAX_NAME_SIZE]; 
}FriendRequest;

typedef struct {
   unsigned int signature: 16;
   unsigned int size: 16; 
   Bool validation;
   //char excess[BUF_SIZE - 1]; 
}Response;

typedef struct {
   unsigned int signature: 16;
   unsigned int size: 16; 
   Bool friends_only;
   //char excess[BUF_SIZE - 1]; 
}SeeActivePlayersRequest;

typedef struct {
   unsigned int signature: 16;
   unsigned int size: 16; 
   Bool friends_only;
   //char excess[BUF_SIZE - 1]; 
}SeeActiveGamesRequest;

typedef struct {
   unsigned int signature: 16;
   unsigned int size: 16; 
   char game_name[BUF_SIZE]; 
}ObserveRequest;

typedef struct {
   unsigned int signature: 16;
   //unsigned int size: 16; 
   //char excess[BUF_SIZE]; 
}QuitRequest;