#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "server.h"

static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if (err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }

#elif defined(__linux__) || defined(linux)
   signal(SIGCHLD,SIG_IGN);

#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

// #region ////////////////////////////////////// - Game - ////////////////////////////////////////////////////////////////

// #region Playing //
void create_game(Client *client1, Client *client2, Bool private)
{
   Game *game = (Game *)malloc(sizeof(Game));
   game->private = private;
   game->game_board = initGame();
   game->clients_involved[0] = client1;
   game->clients_involved[1] = client2;
   client1->player->current_game = client2->player->current_game = game;
   game->nb_observers = 0;
}

Bool accept_challenge(Client *challenged)
{
   Game *game = challenged->player->current_game;
   game->clients_involved[0]->player->player_state = IN_GAME;
   game->clients_involved[1]->player->player_state = IN_GAME;
   return true;
}

void end_game(Game *game)
{
   if(game->clients_involved[0])
   {
      PlayerInfo *player1 = game->clients_involved[0]->player;
      player1->player_state = IDLE;
      player1->current_game = NULL;
   }
   if(game->clients_involved[1])
   {
      PlayerInfo *player2 = game->clients_involved[1]->player;
      player2->player_state = IDLE;
      player2->current_game = NULL;
   }
   for (int i = 0; i < game->nb_observers; ++i)
   {
      game->observers[i]->player->player_state = IDLE;
      game->observers[i]->player->current_game = NULL;
      game->observers[i]->player->observer_index = NON_OBSERVER;
   }
   endGame(game->game_board);
   free(game);
}

// void disconnect_players_from_game(Client * disconnected, const char * message)
// {
//    PlayerInfo* activePlayer = disconnected->player->current_game->clients_involved[0] == disconnected ? disconnected->player->current_game->clients_involved[1]->player : disconnected->player->current_game->clients_involved[0]->player;
   
//    end_game(disconnected->player->current_game);
//    remove_client(index_name_client(disconnected->player->name));

//    write_client(activePlayer->client->sock, message);
// }

Bool make_move(Game *game, NumCase played_house)
{
   if (!play(game->game_board, played_house))
      return false;
   char message[300];
   sprintf(message, "Coup joué : %d\n", played_house + 1);
   write_client(game->clients_involved[0]->sock, message);
   write_client(game->clients_involved[1]->sock, message);
   return true;
}

Bool is_current_player(const Game* game, const Client* client)
{
   return game->clients_involved[game->game_board->joueurCourant - 1] == client;
}

void continue_game(Game *game)
{
   char message[300];
   // Print board and score
   sprintf(message, "Score : %s : %d - %s : %d", game->clients_involved[0]->player->name, game->game_board->grainesJ1, game->clients_involved[1]->player->name, game->game_board->grainesJ2);
   write_client(game->clients_involved[0]->sock, message); // show score
   write_client(game->clients_involved[1]->sock, message);
   print_board_to(JOUEUR1, game, 0);
   print_board_to(JOUEUR2, game, 0);
   for (int o = 0; o < game->nb_observers; o++)
   {
      write_client(game->observers[o]->sock, message);
      print_board_to(OBSERVATEUR, game, o);
   }

   // Test game end
   if (hasWon(game->game_board))
   {
      sprintf(message, "Le joueur %s a gagné la partie!", game->clients_involved[game->game_board->joueurCourant - 1]->player->name);
      write_client(game->clients_involved[0]->sock, message);
      write_client(game->clients_involved[1]->sock, message);
      for (int o = 0; o < game->nb_observers; o++)
         write_client(game->observers[o]->sock, message);
      end_game(game);
      return;
   }
   if (isDraw(game->game_board))
   {
      write_client(game->clients_involved[0]->sock, "Il y a égalité!");
      write_client(game->clients_involved[1]->sock, "Il y a égalité!");
      for (int o = 0; o < game->nb_observers; o++)
         write_client(game->observers[o]->sock, "Il y a égalité!");
      end_game(game);
      return;
   }
   changePlayer(game->game_board);

   // print turn
   sprintf(message, "Au tour de %s", game->clients_involved[game->game_board->joueurCourant - 1]->player->name);
   write_client(game->clients_involved[0]->sock, message);
   write_client(game->clients_involved[1]->sock, message);
   for (unsigned int o = 0; o < game->nb_observers; o++)
      write_client(game->observers[o]->sock, message);

   // Print possible houses to pick for current player
   BitField_1o casesJouables = isOpponentFamished(game->game_board) ? playableFamine(game->game_board) : playable(game->game_board);
   char casesJouablesStr[20];
   bitfieldToString(game->game_board->joueurCourant, casesJouables, casesJouablesStr);
   sprintf(message, "Choissisez une case parmis: %s\n", casesJouablesStr);
   write_client(game->clients_involved[game->game_board->joueurCourant - 1]->sock, message);
}
// #endregion Playing

// #region OBSERVING //
Bool add_observer(Game *game, Client *observer)
{
   if (game->nb_observers >= MAX_OBSERVERS)
      return false; // observer count must not be exceeded
   observer->player->observer_index = game->nb_observers;
   observer->player->current_game = game;
   game->observers[game->nb_observers++] = observer;
   observer->player->player_state = OBSERVING;
   return true;
}

void remove_observer(Game *game, unsigned int o_index)
{
   for (int o = o_index; o < game->nb_observers - 1; o++)
   {
      game->observers[o] = game->observers[o + 1];
   }
   --(game->nb_observers);
}

void quit_observing_game(Client *observer){
   remove_observer(observer->player->current_game, observer->player->observer_index);
   observer->player->current_game = NULL;
   observer->player->observer_index = NON_OBSERVER;
   observer->player->player_state = IDLE;
}
// #endregion Observing

// #region Printing
void bitfieldToString(Joueur JCourant, BitField_1o cases, char *buffer)
{
   NumCase i = JCourant == JOUEUR2 ? 1 : 7;
   NumCase j = 0;
   while (cases)
   {
      if ((cases & 1))
      {
         if (i >= 10)
         {
            buffer[j++] = 48 + i / 10;
            buffer[j++] = 48 + i % 10;
            buffer[j++] = ' ';
         }
         else
         {
            buffer[j++] = 48 + i % 10;
            buffer[j++] = ' ';
         }
      }
      ++i;
      cases >>= 1;
   }
   buffer[j] = '\0';
}

void print_board_to(Joueur dest, const Game* game, unsigned int ob_ind)
{
   char buffer[300];
   if (dest)
   { // player
      printBoard(game->game_board, dest, buffer);
      write_client(game->clients_involved[dest - 1]->sock, buffer);
   }
   else
   { // observer
      printBoard(game->game_board, game->game_board->joueurCourant, buffer);
      write_client(game->observers[ob_ind]->sock, buffer);
   }
}
// #endregion Printing

// #endregion Game

// #region ////////////////////////////////////// - Client/Player Management - ////////////////////////////////////////////

// #region Friends //
void add_friend(PlayerInfo* player1, PlayerInfo* player2){
   player1->friends[player1->friend_count++] = player2;
   player2->friends[player2->friend_count++] = player1;
}

void decline_friend(Client* declined, const char* message){
   PlayerInfo* declinedPlayer = declined->player;
   PlayerInfo* otherPlayer = declined->player->asking_friends;

   declined->player->player_state = IDLE;
   declined->player->asking_friends = NULL;
   write_client(declinedPlayer->client->sock, message);

   if(otherPlayer->player_state == RESPONDING_FRIEND){
      otherPlayer->player_state = IDLE;
      write_client(otherPlayer->client->sock, message);
   }  
   otherPlayer->asking_friends = NULL;
   
}
// #endregion Friends

// #region Utilities //
void manage_timeout(Client *client, unsigned int duration, const State to_check, const char *message, void (*action)(Client *, const char *))
{
   if (!fork())
   {
      sleep(5);
      if (client->player->player_state == to_check){
         action(client, message);
      }
        
      exit(0);
   }
}

int index_name_client(const char name[MAX_NAME_SIZE])
{
   int i;
   for (i = 0; i < actual_clients; i++)
      if (strcmp(clients[i]->player->name, name) == 0)
         break;
   return i;
}

int index_name_player(const char name[MAX_NAME_SIZE])
{
   int i;
   for (i = 0; i < actual_players; i++)
      if (strcmp(players[i]->name, name) == 0)
         break;
   return i;
}

Bool are_friend(const PlayerInfo* player1, const PlayerInfo* player2)
{
   for (int i = 0; i < player1->friend_count; ++i)
      if (player1->friends[i] == player2)
         return true;
   return false;
}

void set_initial_player(PlayerInfo* player){
   player->player_state = IDLE;
   if(player->current_game)
   {
      if(player->observer_index != NON_OBSERVER)
      {
         quit_observing_game(player->client);
      }
      else{
         end_game(player->current_game);
         player->current_game = NULL;
      }
   }
   player->observer_index = NON_OBSERVER;
   player->client = NULL;
   player->asking_friends = NULL;
}
// #endregion Utilities

// #region lecture //
static void write_client(SOCKET sock, const char *buffer)
{
   unsigned int n = strlen(buffer) + strlen(CRLF) - 1;
   char message[n];
   strcpy(message, buffer);
   strcat(message, CRLF);
   if (send(sock, message, n, 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
   sleep_func(MESSAGE_TIME_INTERVAL);
}


static void send_message_to_all_clients(Client *sender, const char *buffer, Bool from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for (i = 0; i < actual_clients; i++)
   {
      /* we don't send message to the sender */
      if (sender->sock != clients[i]->sock)
      {
         if (from_server == 0)
         {
            strncpy(message, sender->player->name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i]->sock, message);
      }
   }
}

static void send_error_message(Client* client, const char *message, const State default_state){
   write_client(client->sock, message);
   if(default_state != UNDEFINED) client->player->player_state = default_state;
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if ((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}
// #endregion lecture

// #endregion Client/player Management

// #region ////////////////////////////////////// - Connexion Management - ////////////////////////////////////////////////

static void disconnect_client(Client * client){
   if(client->player->player_state == IN_GAME)
   {
      client->player->player_state = DISCONNECTED_FGAME;
      PlayerInfo* activePlayer = client->player->current_game->clients_involved[0] == client ? client->player->current_game->clients_involved[1]->player : client->player->current_game->clients_involved[0]->player;
      write_client(activePlayer->client->sock, "Player disconnected, waiting 30 seconds...");
      //manage_timeout(client, 30, DISCONNECTED_FGAME, "Player disconnected. Game is cancelled", disconnect_players_from_game); // client left as a zombie for 30s
   }
   else
   {
      set_initial_player(client->player);
      client->player->player_state = LOGGED_OUT;
   }
   remove_client(index_name_client(client->player->name));
   closesocket(client->sock);
}

static void clear_clients() ////////////////////////////////////////////////////////////////////////////////////////////// revoir if ingames sauvegarder
{
   int i = 0;
   for (i = 0; i < actual_clients; i++)
   {
      free(clients[i]);
      closesocket(clients[i]->sock);
   }
}

static void clear_players() ////////////////////////////////////////////////////////////////////////////////////////////// revoir if ingames sauvegarder
{
   for (int i = 0; i < actual_players; i++) free(players[i]);
}

static void remove_client(int to_remove) ////////////////////////////////////////////////////////////////////////////////////////////// revoir if ingames sauvegarder
{
   /* we remove the client in the array */
   free(clients[to_remove]);
   memmove(clients + to_remove, clients + to_remove + 1, (actual_clients - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (actual_clients)--;
}

static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = {0};

   if (sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if (bind(sock, (SOCKADDR *)&sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if (listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
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
// #endregion

// #region ////////////////////////////////////// - Request Management - //////////////////////////////////////////////////

void read_request(Client *requester, const char *req)
{
   ClientRequest *request = (ClientRequest *)req;
   PlayerInfo *req_player = requester->player; // Player information of the requester
   State requester_state = req_player->player_state;

   switch (request->signature)
   {
   // #region LOGOUT //WIP: question of quick command after logout//
   case LOGOUT: 
      // if (requester_state != IDLE)
      // {
      //    send_error_message(requester, "Error: 'There’s a time and place for everything, but not now.' - Profesor Oak", UNDEFINED);
      //    break; 
      // }
         
      disconnect_client(requester);
      break;
   // #endregion
   
   // #region PROFILE //WIP//
   case PROFILE: 
      if (requester_state != IDLE)
      {
         send_error_message(requester, "Error: 'There’s a time and place for everything, but not now.' - Profesor Oak", UNDEFINED);
         break; 
      }
      req_player->player_state = LISTENING;
      ProfileRequest *profile_req = (ProfileRequest *)request;
      req_player->player_state = requester_state; // stops listening
      break;
   // #endregion

   // #region MESSAGE
   case MESSAGE:
      MessageRequest *message_req = (MessageRequest *)request;
      if (message_req->player)
      {
         int index = index_name_client(message_req->player_name);
         if (index == actual_clients)
         {
            send_error_message(requester, "Error: Player doesn't exist.", UNDEFINED);
            break;
         }
         char message[BUF_SIZE];
         strncpy(message, req_player->name, BUF_SIZE - 1);
         strncat(message, " (whispers) : ", sizeof message - strlen(message) - 1);
         strncat(message, message_req->message, sizeof message - strlen(message) - 1);
         SOCKET dest_socket = clients[index]->sock;
         write_client(dest_socket, message);
      }
      else
         send_message_to_all_clients(requester, message_req->message, false);
      break;
   // #endregion

   // #region CHALLENGE
   case CHALLENGE:
      if (requester_state != IDLE)
      {
         send_error_message(requester, "Error: 'There’s a time and place for everything, but not now.' - Profesor Oak", UNDEFINED);
         break; 
      }
      req_player->player_state = AWAITING_CHALLENGE;
      ChallengeRequest *challenge_req = (ChallengeRequest *)request;
      int challenged_index = index_name_client(challenge_req->player_name); // index of challenged player
      
      if (challenged_index == actual_clients)
      {
         send_error_message(requester, "Error: Player doesn't exist.", IDLE);
         break;
      }
      Client *challenged_client = clients[challenged_index];

      // Tests
      Bool challenged_occupied = challenged_client->player->player_state != IDLE;

      if (challenged_occupied)
      {
         send_error_message(requester, "Error: Player is not available right now", IDLE);
         break;
      }

      // we prepare the game and inform the challenged player
      challenged_client->player->player_state = RESPONDING_CHALLENGE;
      char buffer[BUF_SIZE];
      sprintf(buffer, "You have been challenged by %s.%s Type /accept to accept or /decline to refuse%s", req_player->name, CRLF, CRLF);
      write_client(challenged_client->sock, buffer);

      create_game(requester, challenged_client, challenge_req->private);
      //manage_timeout(requester, 30, AWAITING_CHALLENGE, "Challenge timeout. Game cancelled", cancel_game);
      break;
   // #endregion 

   // #region MOVE
   case MOVE:
      if (requester_state != IN_GAME)
      {
         send_error_message(requester, "Error: 'There’s a time and place for everything, but not now.' - Profesor Oak", UNDEFINED);
         break; 
      }
      MoveRequest *move_req = (MoveRequest *)request;
      Game *game = req_player->current_game;

      if (is_current_player(game, requester))
      {
         if (make_move(game, move_req->played_house))
            continue_game(game);
         else
            send_error_message(requester, "Error:Invalid House has been inputed. Please choose again.", requester_state);
      }
      else
         send_error_message(requester, "Error: Please wait for your turn to play", requester_state);

      break;
   // #endregion

   // #region FRIEND //
   case FRIEND: 
      //Tests
      if (requester_state != IDLE)
      {
         send_error_message(requester, "Error: 'There’s a time and place for everything, but not now.' - Profesor Oak", UNDEFINED);
         break; 
      }
      FriendRequest *friend_req = (FriendRequest *)request;
      unsigned int to_friend_player_index = index_name_client(friend_req->player_name);

      if (to_friend_player_index == actual_clients)
      {
         send_error_message(requester, "Error: Player doesn't exist.", IDLE);
         break;
      }
      PlayerInfo* to_friend = clients[to_friend_player_index]->player;

      if(to_friend->player_state != IDLE)
      {
         send_error_message(requester, "Error: Player is occupied.", IDLE);
         break;
      }
      if(are_friend(req_player, players[to_friend_player_index]))
      {
         send_error_message(requester, "Error: You are already friends with this player.", IDLE);
         break;
      }
      // Changing states
      req_player->player_state = AWAITING_FRIEND;
      req_player->asking_friends = to_friend;
      to_friend->player_state = RESPONDING_FRIEND;
      to_friend->asking_friends = requester->player;

      // Inform both parties
      char friend_request_msg[MAX_NAME_SIZE + 100];
      strcpy(friend_request_msg, req_player->name);
      strcat(friend_request_msg, " wants to be friends... accept or decline ?");
      
      write_client(to_friend->client->sock, friend_request_msg);  
      write_client(requester->sock, "Sent friend request."); 
      //manage_timeout(requester, 30, AWAITING_FRIEND, "Timeout: Friend request.", decline_friend);
      break;
   // #endregion

   // #region RESPOND //WIP: way to remember friend request like game and add_friend()//
   case RESPOND: 
      Response *response = (Response *)request;
      switch (requester->player->player_state)
      {
      case RESPONDING_CHALLENGE:
         if(!requester->player->current_game)
         {
            send_error_message(requester, "Player has disconnected.", IDLE);
            break;
         }
         if (response->validation && accept_challenge(requester))
         {
            continue_game(req_player->current_game);
         }
         else 
         {
            write_client(requester->sock, "Game challenge was declined.");
            write_client(requester->player->current_game->clients_involved[0]->sock, "Game challenge was declined.");
            end_game(requester->player->current_game);
         }
         break;

      case RESPONDING_FRIEND:
         PlayerInfo* askingPlayer = requester->player->asking_friends;
         requester->player->asking_friends = NULL;
         if (response->validation) {
            if (requester->player->friend_count >= MAX_FRIEND_COUNT || askingPlayer->friend_count >= MAX_FRIEND_COUNT) {
               send_error_message(requester, "Error: Max friend count achieved.", IDLE);
               if(askingPlayer->client) send_error_message(askingPlayer->client, "Error: Max friend count achieved.", IDLE);
               break;
            }
            if (are_friend(requester->player, askingPlayer)){
               send_error_message(requester, "Error: You are already friends.", IDLE);
               if(askingPlayer->client) send_error_message(askingPlayer->client, "Error: You are already friends.", IDLE);
               break;
            }
            add_friend(requester->player, askingPlayer); 
            write_client(requester->sock, "Success: Friend added !");
            if(askingPlayer->client) write_client(askingPlayer->client->sock, "Success: Friend added !");
            requester->player->player_state = IDLE; // stops listening
            if(askingPlayer->player_state != LOGGED_OUT) askingPlayer->player_state = IDLE;

         }
         else decline_friend(askingPlayer->client, "Friend request was declined.");
         break;

      default:
         send_error_message(requester, "Error: Nothing to respond to.", requester_state);
      }
      break;
   // #endregion

   // #region ACTIVE_PLAYERS //WIP : test_privacy takes in account 1 player but not friend w both//
   case ACTIVE_PLAYERS:
      if (requester_state != IDLE)
      {
         send_error_message(requester, "Error: 'There’s a time and place for everything, but not now.' - Profesor Oak", UNDEFINED);
         break; 
      }
      req_player->player_state = LISTENING;
      SeeActivePlayersRequest *active_players_req = (SeeActivePlayersRequest *)request;
      write_client(requester->sock, "<-- Active Player Name : State -->");

      char ActivePlayerMessage[MAX_NAME_SIZE + sizeof("In Game") + 1];

      if (active_players_req->friends_only)
      {
         for (int i = 0; i < req_player->friend_count; ++i)
         {
            if (req_player->friends[i]->client != NULL) // if friend connected
            {
               strcpy(ActivePlayerMessage, req_player->friends[i]->name);
               strcat(ActivePlayerMessage, req_player->friends[i]->player_state == IDLE ? " : Ready" : ((req_player->friends[i]->player_state == IN_GAME) ? " : In Game" : " : -"));
               write_client(requester->sock, ActivePlayerMessage);
            }
         }
      }
      else
      {
         for (int i = 0; i < actual_clients; ++i)
         {
            strcpy(ActivePlayerMessage, clients[i]->player->name);
            strcat(ActivePlayerMessage, clients[i]->player->player_state == IDLE ? " : Ready" : ((clients[i]->player->player_state == IN_GAME && (!clients[i]->player->current_game->private || are_friend(clients[i]->player, requester->player))) ? " : In Game" : " : -"));
            write_client(requester->sock, ActivePlayerMessage);
         }
      }
      req_player->player_state = requester_state; // stops listening    
      break;
   // #endregion

   // #region ACTIVE_GAMES //WIP : test_privacy takes in account 1 player but not friend w both//
   case ACTIVE_GAMES: 
      if (requester_state != IDLE)
      {
         send_error_message(requester, "Error: 'There’s a time and place for everything, but not now.' - Profesor Oak", UNDEFINED);
         break; 
      }
      req_player->player_state = LISTENING;
      SeeActiveGamesRequest *actives_games_req = (SeeActiveGamesRequest *)request;
      char ActiveGameMessage[2 * MAX_NAME_SIZE + sizeof(" versus ") + 1];
      write_client(requester->sock, "<-- Active Games -->");

      if (actives_games_req->friends_only)
      {
         for (int i = 0; i < req_player->friend_count; ++i)
         {
            if (req_player->friends[i]->client != NULL && req_player->friends[i]->player_state == IN_GAME) // if friend connected and in game
            {
               Game* game = req_player->friends[i]->current_game;
               strcpy(ActiveGameMessage, game->clients_involved[0]->player->name);
               strcat(ActiveGameMessage, " versus ");
               strcat(ActiveGameMessage, game->clients_involved[1]->player->name);
               write_client(requester->sock, ActiveGameMessage);
            }
         }
      }
      else
      {
         for (int i = 0; i < actual_clients; ++i)
         {
            Bool test_game_and_privacy = clients[i]->player->player_state == IN_GAME ? (!clients[i]->player->current_game->private || are_friend(clients[i]->player, requester->player)) : false;
            if(!test_game_and_privacy) continue;
            Game* game = clients[i]->player->current_game;
            strcpy(ActiveGameMessage, game->clients_involved[0]->player->name);
            strcat(ActiveGameMessage, " versus ");
            strcat(ActiveGameMessage, game->clients_involved[1]->player->name);
            write_client(requester->sock, ActiveGameMessage);
         }
      }
      req_player->player_state = requester_state; // stops listening
      break; 
   // #endregion

   // #region OBSERVE //WIP : test_privacy takes in account 1 player but not friend w both//
   case OBSERVE:
      if (requester_state != IDLE)
      {
         send_error_message(requester, "Error: 'There’s a time and place for everything, but not now.' - Profesor Oak", UNDEFINED);
         break; 
      }
      ObserveRequest *observe_req = (ObserveRequest *)request;

      // Tests
      int index_client_to_observe = index_name_client(observe_req->player_name); // index du joueur qu'on veut observer (son jeu)
      if (index_client_to_observe == actual_clients){
         send_error_message(requester, "Error: No such player.", IDLE);
         break;
      }

      Game *game_to_observe = clients[index_client_to_observe]->player->current_game;
      Bool test_game_exists = clients[index_client_to_observe]->player->player_state == IN_GAME;
      if(! test_game_exists)
      {
         send_error_message(requester, "Error: No game to observe", IDLE);
         break;
      } 
      
      Bool test_privacy = !game_to_observe->private || (are_friend(game->clients_involved[0]->player, requester->player) && are_friend(game->clients_involved[1]->player, requester->player));
      if (test_privacy)
      {
         if (add_observer(clients[index_client_to_observe]->player->current_game, requester)) {
            char message[300];
            sprintf(message, "Score : %s : %d - %s : %d", game_to_observe->clients_involved[0]->player->name, game_to_observe->game_board->grainesJ1, game_to_observe->clients_involved[1]->player->name, game_to_observe->game_board->grainesJ2);
            write_client(requester->sock, message); //score
            print_board_to(OBSERVATEUR, game_to_observe, req_player->observer_index); // board
            sprintf(message, "Au tour de %s", game_to_observe->clients_involved[game_to_observe->game_board->joueurCourant - 1]->player->name);
            write_client(requester->sock, message); //turn
         }
         else
         {
            send_error_message(requester, "Error: Max observer limit achieved in game", IDLE);
         }
      }
      else
      {
         send_error_message(requester, "Error: No game to observe", IDLE);
      }
      break;
   // #endregion

   // #region QUIT
   case QUIT: 
      if (requester_state != OBSERVING)
      {
         send_error_message(requester, "Error: 'There’s a time and place for everything, but not now.' - Profesor Oak", UNDEFINED);
         break; 
      }
      req_player->player_state = LISTENING;
      quit_observing_game(requester);
      write_client(requester->sock, "Success: Quitted game");
      
      break;
   // #endregion

   default:
      // GESTION
      req_player->player_state = LISTENING;
      req_player->player_state = requester_state; // stops listening
      break;
   }
   write_client(requester->sock, CRLF);
}
// #endregion

// #region ////////////////////////////////////// - Server Management - ///////////////////////////////////////////////////

static void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   actual_clients = 0;
   int max = sock;
   /* an array for all clients */

   fd_set rdfs;

   while (1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for (i = 0; i < actual_clients; i++)
      {
         if(clients[i]->player->player_state == DISCONNECTED_FGAME) continue;
         FD_SET(clients[i]->sock, &rdfs);
      }

      if (select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if (FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if (FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = {0};
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if (csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* Max active player count achieved */
         if (actual_clients == MAX_CLIENTS)
         {
            write_client(csock, "Max number of clients achieved");
            continue;
         }

         /* after connecting the client sends its name */
         if (read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         /* Player connexion */
         PlayerInfo* p = (PlayerInfo*) malloc(sizeof(PlayerInfo));
         p->friend_count = 0;
         p->current_game = NULL;
         set_initial_player(p);
         

         strncpy(p->name, buffer, MAX_NAME_SIZE - 1); /////////////// TO DO : à vérifier + BIO !!!!
         strncpy(p->password, buffer, MAX_PASSWORD_SIZE - 1);

         int index_player = index_name_player(p->name);
         if (index_player == actual_players && actual_players != MAX_PLAYER_COUNT) // New Player
         {
            players[actual_players] = p;
            ++actual_players;
         }
         else
         {
            if (strcmp(players[index_player]->password, p->password) != 0) { // Wrong password
               free(p);
               continue;
            }
            else { // player exists
               free(p);
               p = players[index_player];
               p->player_state = IDLE;
            }
         }

         Client* c = (Client*) malloc(sizeof(Client));
         c->sock = csock;
         c->player = p;

         // Player connexion
         int index_client = index_name_client(p->name);
         if (index_client != actual_clients) // client already connected ?
         {
            closesocket(csock);
            free(c);
            continue;
         }
         else // new client
         {
            clients[actual_clients] = c;
            p->client = c;
            ++actual_clients;
            /* what is the new maximum fd ? */
            max = csock > max ? csock : max;
            FD_SET(csock, &rdfs);
            if(p->player_state == DISCONNECTED_FGAME) // disconected forcibly from game and timeout still active
            { 
               p->player_state = IN_GAME; 
               write_client(c->sock, "Welcome back!");
               Game* game = p-> current_game;
               Joueur absent_player = game->clients_involved[0] ? JOUEUR2 : JOUEUR1;
               game->clients_involved[absent_player - 1] = c;
               // We re-inform of game state
               print_board_to(absent_player, game, 0);
               
               // We inform the other player
               Client* other_client = clients[index_client] == game->clients_involved[0] ? game->clients_involved[1] : game->clients_involved[0];
               write_client(other_client->sock, "Player reconnected !");
            }
         }
         
      }
      else
      {
         int i = 0;
         for (i = 0; i < actual_clients; i++)
         {
            /* a client is talking */
            if (FD_ISSET(clients[i]->sock, &rdfs))
            {
               Client *client = clients[i];
               int c = read_client(client->sock, buffer);
               /* client disconnected */
               if (c == 0)
               {
                  disconnect_client(client);
               }
               else
               {
                  read_request(client, buffer);
               }
               break;
            }
         }
      }
   }

   clear_clients();
   clear_players(); // no database yet
   end_connection(sock);
}
// #endregion

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
