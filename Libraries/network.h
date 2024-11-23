#pragma once

#ifdef WIN32

#include <winsock2.h>
#include <windows.h>
#define sleep_func(ms) Sleep(ms)

#elif defined(__linux__) || defined(linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h>  /* gethostbyname */
#include <sys/select.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
#define sleep_func(ms) usleep((ms)*1000)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

// #region Server parameters //

#define CRLF "\r\n"
#define PORT 1977
#define MAX_CLIENTS 100
#define MAX_OBSERVERS (MAX_CLIENTS - 2)

#define BUF_SIZE 1024
#define MESSAGE_TIME_INTERVAL 1 // in milliseconds
#define STD_TIMEOUT_DURATION 30000 // in milliseconds

#define MAX_PLAYER_COUNT 1024
#define MAX_FRIEND_COUNT MAX_PLAYER_COUNT
#define MAX_NAME_SIZE 50
#define MAX_PASSWORD_SIZE 50
#define MAX_BIO_SIZE 50

// #endregion Server parameters