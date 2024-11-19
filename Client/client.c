#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "client.h"

static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}


ClientRequest* create_request(const char* buffer){
   ClientRequest* request = (ClientRequest*) malloc(sizeof(ClientRequest));
   request->size = 0;
   char command[50]; // To store the part after '/'
   char rest[BUF_SIZE - 52];    // To store the part after the first space
   char *spacePtr;   // Pointer to the first space

   // Check if the input starts with '/'
   if (buffer[0] == '/') {
      // Skip the '/' and find the first space
      spacePtr = strchr(buffer + 1, ' ');

      if (spacePtr != NULL) {
         // Split the input at the first space
         *spacePtr = '\0';            // Replace the space with a null terminator
         strcpy(command, buffer + 1); // Copy the command part
         strcpy(rest, spacePtr + 1);  // Copy the rest of the string
      }
      else {
         // No space found, so everything after '/' is the command
         strcpy(command, buffer + 1);
         rest[0] = '\0'; // Make `rest` an empty string
      }
      
      if (strcmp(command, "help") == 0 || strcmp(command, "?") == 0) {
         printf("/logout                              : to quit the server\n");
         printf("/msg <player-name> <message-content> : to send a private message\n");
         printf("/challenge <player-name>             : to challenge a friend\n");
         printf("/play [online, private]              : to play against a bot or a player, privately or not\n");
         printf("/move <house-number>                 : to choose a move to play\n");
         printf("/friend <player-name>                : to add a friend\n");
         printf("/accept                              : to accept a request\n");
         printf("/decline                             : to decline a request\n");
         printf("/who [friend]                        : to see all online players or only you friends\n");
         printf("/games [friend]                      : to see all active games or only your friend's games\n");
         printf("/observe <player-name>               : to observe a friend's game\n");
         printf("/quit                                : to quit observing a game\n");
      }
      else if (strcmp(command, "logout") == 0) {
         request->signature = LOGOUT;
         request->size = 2 + 2;
      }
      else if (strcmp(command, "msg") == 0) {
         if (rest[0] == '\0'){
            printf("To use the /msg command, you need to indicate the player name then the message you want to send.\n/msg <player-name> <message-content>\n");
            return request;
         }
         char *sSpacePtr = strchr(spacePtr + 1, ' ');
         if (sSpacePtr == NULL) {
            printf("To use the /msg command, you need to indicate the player name then the message you want to send.\n/msg <player-name> <message-content>\n");
            return request;
         }
         char msg[BUF_SIZE - 52];
         *sSpacePtr = '\0';
         strcpy(rest, spacePtr + 1);
         strcpy(msg, sSpacePtr + 1);
         if (msg[0] == '\0') {
            printf("To use the /msg command, you need to indicate the player name then the message you want to send.\n/msg <player-name> <message-content>\n");
            return request;   
         }
         request->signature = MESSAGE;
         MessageRequest *message_request = (MessageRequest *)request;
         message_request->player = true;
         strcpy(message_request->player_name, rest);
         strcpy(message_request->message, msg);
         message_request->size = 2 + 2 + 1 + MAX_NAME_SIZE + strlen(msg);
      }
      else if (strcmp(command, "challenge") == 0) {
         if (rest[0] == '\0'){
            printf("To use the /challenge command, you need to indicate the name of the player that you want to challenge, and then indicate if you want to have this game private (only friends can observe).\n/challenge <player-name> [private]\n");
            return request;
         }
         char *sSpacePtr = strchr(spacePtr + 1, ' ');
         char private[BUF_SIZE - 53];
         if (sSpacePtr != NULL) {
            *sSpacePtr = '\0';
            strcpy(private, sSpacePtr + 1);
         }
         request->signature = CHALLENGE;
         ChallengeRequest *challenge_request = (ChallengeRequest *)request;
         if (sSpacePtr != NULL && strcmp(private, "private") == 0)
            challenge_request->private = true;
         else if (sSpacePtr == NULL || private[0] == '\0')
            challenge_request->private = false;
         else {
            printf("To use the /challenge command, you need to indicate the name of the player that you want to challenge, and then indicate if you want to have this game private (only friends can observe).\n/challenge <player-name> [private]\n");
            return request;
         }
         strcpy(challenge_request->player_name, rest);
         challenge_request->size = 2 + 2 + 1 + MAX_NAME_SIZE;
      }
      else if (strcmp(command, "play") == 0) {
         request->signature = PLAY;
         PlayRequest* play_request = (PlayRequest*) request;
         play_request->online = false;
         play_request->private = false;
         char *sSpacePtr = strchr(spacePtr + 1, ' ');
         if (sSpacePtr != NULL) {
            *sSpacePtr = '\0';
            char param[BUF_SIZE - 6];
            strcpy(param, sSpacePtr + 1);
            if (strcmp(param, "private") == 0)
               play_request->private = true;
            else if (strcmp(param, "online") == 0)
               play_request->online = true;
            else if (param[0] != '\0') {
               printf("To use the /play command, you need to indicate if you to play offline (with a bot) or on online, and if you want to have the game in private (they must be seperated with a space bar).\n/play [online, private]\n");
               return request;
            }
         }
         if (strcmp(rest, "private") == 0)
            play_request->private = true;
         else if (strcmp(rest, "online") == 0)
            play_request->online = true;
         else {
            printf("To use the /play command, you need to indicate if you to play offline (with a bot) or on online, and if you want to have the game in private (they must be seperated with a space bar).\n/play [online, private]\n");
            return request;
         }
         play_request->size = 2 + 2 + 1 + 1;
      }
      else if (strcmp(command, "move") == 0) {
         request->signature = MOVE;
         NumCase numero = atoi(rest);
         if (numero == 0) {
            printf("To use the /move command, you need to indicate the number of the house you want to play.\n/move <house-number>\n");
            return request;
         }
         MoveRequest* move_request = (MoveRequest*) request;
         move_request->played_house = numero;
         move_request->size = 2 + 2 + 4;
      }
      else if (strcmp(command, "friend") == 0) {
         if (rest[0] == '\0'){
            printf("To use the /friend command, you need to indicate the player name you want to befriend with.\n/friend <player-name>\n");
            return request;
         }
         request->signature = FRIEND;
         FriendRequest* friend_request = (FriendRequest*)request;
         strcpy(friend_request->player_name, rest);
         friend_request->size = 2 + 2 + MAX_NAME_SIZE;
      }
      else if (strcmp(command, "accept") == 0) {
      request->signature = RESPOND;
      Response* response_request = (Response*) request;
      response_request->validation = true;
      request->size = 2 + 2 + 1;
      }
      else if (strcmp(command, "decline") == 0) {
      request->signature = RESPOND;
      Response* response_request = (Response*) request;
      response_request->validation = false;
      request->size = 2 + 2 + 1;
      }
      else if (strcmp(command, "who") == 0) { ///////////
         request->signature = ACTIVE_PLAYERS;
         SeeActivePlayersRequest* player_request = (SeeActivePlayersRequest*) request;
         if (strcmp(rest, "friend") == 0)
            player_request->friends_only = true;
         else if (rest[0] == '\0')
            player_request->friends_only = false;
         else {
            printf("To use the /who command, you need to indicate if you want to see only your active friend or all active player.\n/who [friend]\n");
            return request;
         }
         request->size = 2 + 2 + 1;
      }
      else if (strcmp(command, "games") == 0) { //////////
         request->signature = ACTIVE_GAMES;
         SeeActiveGamesRequest* games_request = (SeeActiveGamesRequest*) request;
         if (strcmp(rest, "friend") == 0)
            games_request->friends_only = true;
         else if (rest[0] == '\0')
            games_request->friends_only = false;
         else {
            printf("To use the /games command, you need to indicate if you want to see only active games that your friends are playing.\n/games [friend]\n");
            return request;
         }
         request->size = 2 + 2 + 1;
      }
      else if (strcmp(command, "observe") == 0) {
         if (rest[0] == '\0'){
            printf("To use the /observe command, you need to indicate the name of the player that you want to watch its game.\n/observe <player-name>\n");
            return request;
         }
         request->signature = OBSERVE;
         ObserveRequest* observe_request = (ObserveRequest*)request;
         strcpy(observe_request->player_name, rest);
         observe_request->size = 2 + 2 + MAX_NAME_SIZE;
      }
      else if (strcmp(command, "quit") == 0) {
         request->signature = QUIT;
         request->size = 2 + 2;
      }
      else 
         printf("Commande non reconnue. Faites /help ou /? pour voir les commandes existantes.\n");
   }
   else {
      request->signature = MESSAGE;
      MessageRequest* message_request = (MessageRequest*) request;
      message_request->player = false;
      strcpy(message_request->message, rest);
      message_request->size = 2 + 2 + 1 + MAX_NAME_SIZE + strlen(rest);
   }
   return request;
}


void delete_request(ClientRequest* request){
   free(request);
}


static void app(const char *address, const char *name)
{
   SOCKET sock = init_connection(address);
   char buffer[BUF_SIZE];

   fd_set rdfs;

   /* send our name */
   write_server(sock, name);

   while(1)
   {
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the socket */
      FD_SET(sock, &rdfs);

      if(select(sock + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         fgets(buffer, BUF_SIZE - 1, stdin);
         {
            char *p = NULL;
            p = strstr(buffer, "\n");
            if(p != NULL)
            {
               *p = 0;
            }
            else
            {
               /* fclean */
               buffer[BUF_SIZE - 1] = 0;
            }
         }
         ClientRequest* request = create_request(buffer);
         if (request->size != 0)
            write_server_request(sock, request);
         delete_request(request);
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         int n = read_server(sock, buffer);
         /* server down */
         if(n == 0)
         {
            printf("Server disconnected !\n");
            break;
         }
         puts(buffer);
      }
   }

   end_connection(sock);
}

static int init_connection(const char *address)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };
   struct hostent *hostinfo;

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   hostinfo = gethostbyname(address);
   if (hostinfo == NULL)
   {
      fprintf (stderr, "Unknown host %s.\n", address);
      exit(EXIT_FAILURE);
   }

   sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr_list[0];
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
   {
      perror("connect()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_server(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      exit(errno);
   }

   buffer[n] = 0;

   return n;
}

static void write_server(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

static void write_server_request(SOCKET sock, const ClientRequest *request)
{
   if(send(sock, request, 1024, 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv)
{
   if(argc < 2)
   {
      printf("Usage : %s [address] [pseudo]\n", argv[0]);
      return EXIT_FAILURE;
   }

   init();

   app(argv[1], argv[2]);

   end();

   return EXIT_SUCCESS;
}
