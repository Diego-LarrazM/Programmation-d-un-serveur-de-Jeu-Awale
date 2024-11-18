#pragma once

#ifdef WIN32

#include <winsock2.h>

#elif defined(linux)

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
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF "\r\n"
#define PORT 1977
#define MAX_CLIENTS 100
#define MAX_OBSERVERS (MAX_CLIENTS - 2)

#define BUF_SIZE 1024

#define MAX_PLAYER_COUNT 1024
#define MAX_FRIEND_COUNT MAX_PLAYER_COUNT
#define MAX_NAME_SIZE 50
#define MAX_PASSWORD_SIZE 50
#define MAX_BIO_SIZE 50
#define MAX_FRIEND_REQUEST_SIZE MAX_PLAYER_COUNT
