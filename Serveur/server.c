#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

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


void create_game(Client* client1, Client* client2){
   Game* game = (Game*) malloc(sizeof(Game));
   game->game_board = initGame();
   game->clients_involved[0] = client1;
   game->clients_involved[1] = client2;
   client1->player->current_game = client2->player->current_game = game;
   game->nbObservers = 0;
}

Bool accept_challenge(Client* challenged){
   Game* game = challenged->player->current_game;
   if(game->clients_involved[0]->player->player_state != AWAITING_CHALLENGE) return false; // Test: Player always ready (not disconected or timeout)
   game->clients_involved[0]->player->player_state = IN_GAME;
   game->clients_involved[1]->player->player_state = IN_GAME;
   return true;
}


void bitfieldToString(Joueur JCourant, BitField_1o cases, char* buffer){
  NumCase i = JCourant == JOUEUR2 ? 1 : 7;
  NumCase j = 0;
    while(cases){
      if(cases & 1){
        if(i >= 10){
          buffer[j++] = 48 + i/10;
          buffer[j++] = 48 + i%10;
          buffer[j++] = ' ';
        }
        else{
          buffer[j++] = 48 + i%10;
          buffer[j++] = ' ';
        }
      }  
      ++i;
      cases >>= 1;
    }
    buffer[j] = '\0';
}


void continue_game(Game* game){
   char message[300];
   printBoard(game->game_board, JOUEUR1, message);
   write_client(game->clients_involved[0]->sock, message);
   printBoard(game->game_board, JOUEUR2, message);
   write_client(game->clients_involved[1]->sock, message);
   
   sprintf(message, "Score : %s : %d - %s : %d\n", game->clients_involved[0]->player->name, game->game_board->grainesJ1, game->clients_involved[1]->player->name, game->game_board->grainesJ2);
   write_client(game->clients_involved[0]->sock, message);
   write_client(game->clients_involved[1]->sock, message);

   if (hasWon(game->game_board)){
      sprintf(message, "Le joueur %s a gagné la partie!\n", game->game_board->JoueurCourant == JOUEUR1 ? game->clients_involved[0]->player->name : game->clients_involved[1]->player->name);
      write_client(game->clients_involved[0]->sock, message);
      write_client(game->clients_involved[1]->sock, message);
      end_game(game);
      return;
   }
   if (isDraw(game->game_board)){
      write_client(game->clients_involved[0]->sock, "Il y a égalité!\n");
      write_client(game->clients_involved[1]->sock, "Il y a égalité!\n");
      end_game(game);
      return;
   }

   changePlayer(game->game_board);

   sprintf(message, "Au tour de %s\n", game->game_board->JoueurCourant == JOUEUR1 ? game->clients_involved[0]->player->name : game->clients_involved[1]->player->name);
   write_client(game->clients_involved[0]->sock, message);
   write_client(game->clients_involved[1]->sock, message);

   BitField_1o casesJouables = isOpponentFamished(game->game_board) ? playableFamine(game->game_board) : 63;
   char casesJouablesStr[20];
   bitfieldToString(game->game_board->JoueurCourant, casesJouables, casesJouablesStr);
   sprintf(message, "Choissisez une case parmis: %s\n", casesJouablesStr);

   /*
   TO DO LEFT: il manque l'affichage pour les observateurs
   -> faire fonction pour print un message aux deux joueurs ?
   -> faire fonction pour print un message aux observeurs ?
   -> faire fonction pour print un message aux deux joueurs + observeurs ?
   */


   /*TO DO: affichage, indiquer joueur courant, cases jouables, voir qui gagne et arret etc*/
   //players->player_state = LISTENING;
   //players->player_state = IN_GAME;
}

Bool make_move(Game* game, NumCase played_house){
   return play(game->game_board, played_house);
   /*TO DO: realiser le move indiqué*/
   //players->player_state = LISTENING;
   //players->player_state = IN_GAME;
}

Bool is_current_player(Game* game, Client* client){
   if (game->game_board->JoueurCourant == JOUEUR1 && game->clients_involved[0] == client)
      return true;
   if (game->game_board->JoueurCourant == JOUEUR2 && game->clients_involved[1] == client)
      return true;
   return false;
}


void end_game(Game* game){
   PlayerInfo* player1 =  game->clients_involved[0]->player;
   player1->player_state = IDLE;
   player1->current_game = NULL;
   PlayerInfo* player2 =  game->clients_involved[1]->player;
   player2->player_state = IDLE;
   player2->current_game = NULL;

   endGame(game->game_board);
   for (int i = 0; i < game->nbObservers; ++i){
      game->observers[i]->player->player_state = IDLE;
   }
   free(game);
}

void cancel_game(Client* client, char* message){
   write_client(client->sock, message);
   end_game(client->player->current_game);
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


void read_request(Client* clients, Client* requester, int actual_clients, const char* req){
   ClientRequest* request = (ClientRequest*)req;
   PlayerInfo* req_player = requester->player; // Player information of the requester

   switch (request->signature){///////////////////////////////////////////////////////////////////////////////////////////ADD LISTENING STATE TO ALL REQUESTS for requester then back
      case LOGOUT: ////
         break;



      case PROFILE: ////
         if(req_player->player_state != IDLE) break;
         ProfileRequest* profile_req = (ProfileRequest*) request;
         break;



      case MESSAGE: 
         MessageRequest* message_req = (MessageRequest*) request;
         if(message_req->player) {
            SOCKET dest_socket = clients[index_name_client(clients, actual_clients, message_req->player_name)].sock;
            write_client(dest_socket, message_req->message);
         }
         else
            send_message_to_all_clients(clients, requester, actual_clients, message_req->message, 0);
         break;



      case CHALLENGE: 
         if(req_player->player_state != IDLE) break;
         req_player->player_state = AWAITING_CHALLENGE;
         ChallengeRequest* challenge_req = (ChallengeRequest*) request;

         int challenged_index = index_name_client(clients, actual_clients, challenge_req->player_name); // index du joueur qu'on défie
         if(challenged_index == actual_clients || clients[challenged_index].player->player_state != IDLE){ // s'il n'est pas connecté ou est occupé passer
            write_client(requester->sock, "Player is not available right now");
            req_player->player_state = IDLE;
            break;
         }

         clients[challenged_index].player->player_state = RESPONDING_CHALLENGE;

         // message to indicate the challenged player he's been challenged
         char buffer[BUF_SIZE]; 
         sprintf(buffer, "You have been challenged by %s.\n Type ... to accept or ... to refuse\n", req_player->name); ////////////////////// Commandes à ajouter ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         write_client(clients[challenged_index].sock, buffer);

         create_game(requester, &clients[challenged_index]);
         if(!fork()){ ////////////////////// ATTENTION ZOMBIE /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            sleep(30); // Timeout 30s
            if(req_player->player_state == AWAITING_CHALLENGE) 
               cancel_game(requester, "Challenge timeout. Game cancelled.");
         }
         break;



      case PLAY: ////
         if(req_player->player_state != IDLE) break;
         PlayRequest* play_req = (PlayRequest*) request;
         break;



      case MOVE: ////
         if(req_player->player_state != IN_GAME) break;
         MoveRequest* move_req = (MoveRequest*) request;
         Game* game = req_player->current_game;

         if (is_current_player(game, requester)) {
            if (make_move(game, move_req->played_house))
               continue_game(game);
            else
               write_client(requester->sock, "An uncorrect House has been inputed");
            }
         else 
            write_client(requester->sock, "Please wait for your turn to play");
         break;



      case FRIEND: ////
         FriendRequest* friend_req = (FriendRequest*) request;
         break;



      case RESPOND: ////
         Response* response = (Response*) request;
         switch (response->response_type)
         {
         case CHALLENGE_RESPONSE: 
            Response_Challenge* resp_challenge = (Response_Challenge*) response;
            if(resp_challenge->validation && accept_challenge(requester)) continue_game(req_player->current_game);
            else cancel_game(requester, "Game is cancelled.");
            break;

      
         case FRIEND_RESPOND:
            break;
         }
         break;



      case ACTIVE_PLAYERS: ////
         if(req_player->player_state != IDLE) break;
         SeeActivePlayersRequest* active_players_req = (SeeActivePlayersRequest*) request;
         req_player->player_state = LISTENING;

         char ActivePlayerMessage[MAX_NAME_SIZE + sizeof("In Game")]; 

         if (active_players_req->friends_only){
            for (int i = 0; i < req_player->friend_count; ++i) {
               if (req_player->friends[i]->client != NULL)  { // if friend connected
                  strcpy(ActivePlayerMessage, req_player->friends[i]->name);
                  strcat(ActivePlayerMessage, req_player->friends[i]->player_state == IDLE ? "Ready" : ((req_player->friends[i]->player_state == IN_GAME  &&  testPrivacy)? "In Game" : "-"));
                  write_client(requester->sock, req_player->friends[i]->name);
               }
            }
         }
         else{
            for (int i = 0; i < actual_clients; ++i) {
               strcpy(ActivePlayerMessage, clients[i].player->name);
               strcat(ActivePlayerMessage, clients[i].player->player_state == IDLE ? "Ready" : ((clients[i].player->player_state == IN_GAME &&  testPrivacyy)? "In Game" : "-"));
               write_client(requester->sock, clients[i].player->name);
            }
         }
         req_player->player_state = IDLE;
         break;



      case ACTIVE_GAMES: ////
         if(req_player->player_state != IDLE) break;
         SeeActiveGamesRequest* actives_games_req = (SeeActiveGamesRequest*) request;
         break;



      case OBSERVE: ////
         if(req_player->player_state != IDLE) break;
         ObserveRequest* observe_req = (ObserveRequest*) request;

         int index_client_to_observe = index_name_client(clients, actual_clients, challenge_req->player_name); // index du joueur qu'on veut observer (son jeu)
         if(index_client_to_observe != actual_clients && clients[index_client_to_observe].player->player_state == IN_GAME && TestPrivacy){
            add_observer(clients[index_client_to_observe].player->current_game, requester); ////////////////////// ADD CURRENT GAME TO OBSERVERS FOR QUIT
         }
         else write_client(requester->sock, "No game to observe");
         break;


      case QUIT: ////
         if(req_player->player_state != OBSERVING) break;
         if(remove_observer(requester)) write_client(requester->sock, "Quited succesfully");
         else write_client(requester->sock, "Error occured when quitting"); //////////////////////////////////////////////////////////////////// Gestion erreurs avec message ???
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
         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Reverifier les valeurs par defaut
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
         else { // was in game and disconnected forcibly ////////////////////////////////////////////////////////////////////////////////////////////// revoir si bon et timeout
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
               Client* client = &clients[i];
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */ ////////////////////////////////////////////////////////////////////////////////////////////// Attention si IN_GAME à ajouter mettre disconected au player state
               if(c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual_clients);
                  strncpy(buffer, client->player->name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, client, actual_clients, buffer, 1);
               }
               else
               {
                  //send_message_to_all_clients(clients, &client, actual_clients, buffer, 0);
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

static void clear_clients(Client *clients, int actual_clients) ////////////////////////////////////////////////////////////////////////////////////////////// revoir if ingames sauvegarder
{
   int i = 0;
   for(i = 0; i < actual_clients; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual_clients) ////////////////////////////////////////////////////////////////////////////////////////////// revoir if ingames sauvegarder
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual_clients - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual_clients)--;
}

static void send_message_to_all_clients(Client *clients, Client* sender, int actual_clients, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual_clients; i++)
   {
      /* we don't send message to the sender */
      if(sender->sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender->player->name, BUF_SIZE - 1);
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
