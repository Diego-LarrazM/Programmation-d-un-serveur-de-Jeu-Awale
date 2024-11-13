#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
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


int index_name_client(Client clients[MAX_CLIENTS], int currentCount, char name[MAX_NAME_SIZE]) {
    int i;
    for (i = 0; i < currentCount; i++)
        if (strcmp(clients[i].player->name, name) == 0)
            break;
    return i;
}


int index_name_player(PlayerInfo players[MAX_PLAYER_COUNT], int currentCount, char name[MAX_NAME_SIZE]) {
    int i;
    for (i = 0; i < currentCount; i++)
        if (strcmp(players[i].name, name) == 0)
            break;
    return i;
}


Bool are_friend(PlayerInfo* player1, PlayerInfo* player2){
   for (int i = 0; i < player1->friend_count; ++i)
      if (player1->friends[i] == player2)
         return true;
   return false;
}


void tick_TLLs(PlayerInfo* player){
   
}

void read_request(Client *clients, Client client, int actual_clients, const char* req){
   ClientRequest* request = req;

   switch (request->signature){
      case LOGOUT: ////
         break;
      case PROFILE: ////
         if(client.player->player_state != IDLE) break;
         ProfileRequest* profile_req = request;
         break;
      case MESSAGE: /**/
         MessageRequest* message_req = request;
         if(message_req->player) {
            SOCKET dest_socket = clients[index_name_client(clients, actual_clients, message_req->player_name)].sock;
            write_client(dest_socket, message_req->message);
         }
         else
            send_message_to_all_clients(clients, client, actual_clients, message_req->message, 0);
         break;
      case CHALLENGE: ////
         if(client.player->player_state != IDLE) break;
         
         client.player->player_state = AWAITING_CHALLENGE;
         ChallengeRequest* challenge_req = request;
         int challenged_index = index_name_client(clients, actual_clients, challenge_req->player_name);

         if(challenged_index == actual_clients || clients[challenged_index].player->player_state != IDLE){
            write_client(client.sock, "Player is not available right now");
            client.player->player_state = IDLE;
            break;
         }
         // GESTION DU JEU

         
         break;
      case PLAY: ////
         if(client.player->player_state != IDLE) break;
         PlayRequest* play_req = request;
         break;
      case MOVE: ////
         if(client.player->player_state != IN_GAME) break;
         MoveRequest* move_req = request;
         break;
      case FRIEND: ////
         FriendRequest* friend_req = request;
         break;
      case RESPOND_FRIEND: ////
         RespondFriendRequest* respond_req = request;
         break;
      case ACTIVE_PLAYERS:
         if(client.player->player_state != IDLE) break;
         SeeActivePlayersRequest* active_players_req = request;
         client.player->player_state = LISTENING;

         char ActivePlayerMessage[MAX_NAME_SIZE + sizeof("Ready")];

         if (active_players_req->friends_only){
            for (int i = 0; i < client.player->friend_count; ++i) {
               if (client.player->friends[i]->client != NULL)  {
                  strcpy(ActivePlayerMessage, client.player->friends[i]->name);
                  strcat(ActivePlayerMessage, client.player->friends[i]->player_state == IDLE ? "Ready" : " -");
                  write_client(client.sock, client.player->friends[i]->name);
               }
            }
         }
         else{
            for (int i = 0; i < actual_clients; ++i) {
               strcpy(ActivePlayerMessage, clients[i].player->name);
               strcat(ActivePlayerMessage, clients[i].player->player_state == IDLE ? "Ready" : " -");
               write_client(client.sock, clients[i].player->name);
            }
         }
         client.player->player_state = IDLE;
         break;
      case ACTIVE_GAMES: ////
         if(client.player->player_state != IDLE) break;
         SeeActiveGamesRequest* actives_games_req = request;
         break;
      case OBSERVE: ////
         if(client.player->player_state != IDLE) break;
         ObserveRequest* observe_req = request;
         break;
      case QUIT: ////
         if(client.player->player_state == IDLE) break;
         break;
      default:
         // GESTION ERREUR
         break;
   }
}


static void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual_clients = 0;
   int actual_players = 0;
   int max = sock;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];
   PlayerInfo players[MAX_PLAYER_COUNT];

   fd_set rdfs;

   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for(i = 0; i < actual_clients; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* Max active player count achieved */
         if(actual_clients == MAX_CLIENTS)
         {
            continue;
         }

         /* after connecting the client sends its name */
         if(read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         /* Player connexion */
         PlayerInfo p;
         p.player_state = IDLE;
         p.friend_count = 0;
         
         strncpy(p.name, buffer, MAX_NAME_SIZE - 1);
         strncpy(p.password, buffer, MAX_PASSWORD_SIZE - 1);

         int index_player = index_name_player(players, actual_players, p.name);
         if (index_player == actual_players && actual_players != MAX_PLAYER_COUNT){ // New Player
            players[actual_players] = p;
            ++actual_players;
         }
         else {
            if (strcmp(players[index_player].password, p.password) != 0) // Wrong password
               continue;
            else
               p = players[index_player];
         }

         Client c = { csock };
         c.player = &p;

         /* Player already connected ? */
         int index_client = index_name_client(clients, actual_clients, p.name);
         if (index_client != actual_clients && clients[index_client].player->player_state != DISCONNECTED_FGAME)
            continue;
         if (index_client == actual_clients) { // not connected
            clients[actual_clients] = c;
            ++actual_clients;
         }
         else { // was in game and disconnected forcibly
            clients[actual_clients].player->player_state = IN_GAME;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual_clients; i++)
         {
            /*Timeouts */
            tick_TLLs(clients[i].player);
            
            /* a client is talking */
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client client = clients[i];
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */
               if(c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual_clients);
                  strncpy(buffer, client.player->name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, client, actual_clients, buffer, 1);
               }
               else
               {
                  //send_message_to_all_clients(clients, client, actual_clients, buffer, 0);
                  read_request(clients, client, actual_clients, buffer);
               }
               break;
            }
         }
      }
   }

   clear_clients(clients, actual_clients);
   end_connection(sock);
}

static void clear_clients(Client *clients, int actual_clients)
{
   int i = 0;
   for(i = 0; i < actual_clients; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual_clients)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual_clients - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual_clients)--;
}

static void send_message_to_all_clients(Client *clients, Client sender, int actual_clients, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual_clients; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.player->name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
   }
}

static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

static void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
