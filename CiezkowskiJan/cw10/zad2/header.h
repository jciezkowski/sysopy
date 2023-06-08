#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <mqueue.h>
#include <sys/un.h>
#include <pthread.h>

#define UNIX_PATH_MAX 108

#define MAX_USR_INPUT 3000
#define SERVER_ID 1
#define MAX_NO_CLIENTS 16
#define MAX_CLIENT_NAME_LEN 30
#define MAX_NO_EVENTS 20

typedef enum msg_type{
    LIST = 1,
    TOALL = 2,
    TOONE = 3,
    STOP = 4,
    FULL = 5,
    TAKEN = 6,
    PING = 7,
    HELLO = 8,
    ERROR = 9
} msg_type;

typedef union addr {
    struct sockaddr_un addr_un;
    struct sockaddr_in addr_in;
} addr;

typedef struct msg_buffer
{
    msg_type type;
    union addr addr;
    socklen_t addrlen;
    char from[MAX_CLIENT_NAME_LEN + 1];
    char to[MAX_CLIENT_NAME_LEN + 1];
    char mtext[MAX_USR_INPUT];
    struct tm time_struct;
} msg_buffer;

typedef struct client {
    int fd;
    char name[MAX_CLIENT_NAME_LEN + 1];
    int is_active;
    union addr addr;
    socklen_t addrlen;
} client;
