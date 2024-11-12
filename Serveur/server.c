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

void read_request(){
   
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

         /* after connecting the client sends its name */
         if(read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = { csock };
         PlayerInfo p;
         p.playerState = IDLE;
         c.player = &p;
         strncpy(p.name, buffer, MAX_NAME_SIZE - 1);
         strncpy(p.password, buffer, MAX_PASSWORD_SIZE - 1);

         int index_player = index_name_player(players, actual_players, p.name);
         if (index_player == actual_players){ // New Player
            players[actual_players] = p;
            ++actual_players;
         }
         else if (strcmp(players[index_player].password, p.password) != 0) // Wrong password
            continue;

         int index_client = index_name_client(clients, actual_clients, p.name);
         if (index_client != actual_clients && clients[index_client].player->playerState != DISCONNECTED_FGAME)
            continue;
         if (index_client == actual_clients) {
            clients[actual_clients] = c;
            ++actual_clients;
         }
         else {
            clients[actual_clients].player->playerState = IN_GAME;
         }
         //////////////////////////////////////////////////////////////////////////////////// CONNECTING CLIENT
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual_clients; i++)
         {
            /* a client is talking */
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client client = clients[i];
               int c = read_client(clients[i].sock, buffer);r
               /* client disconnected */
               if(c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual_clients);
                  strncpy(buffer, client.name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, client, actual_clients, buffer, 1);
               }
               else
               {
                  //send_message_to_all_clients(clients, client, actual_clients, buffer, 0);
                  read_request(client, buffer, c)
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
            strncpy(message, sender.name, BUF_SIZE - 1);
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
