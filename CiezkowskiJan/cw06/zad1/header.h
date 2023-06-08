#include <stdio.h>
#include <stdlib.h>
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

#define MAX_USR_INPUT 3000
#define SERVER_ID 1
#define MAX_NO_CLIENTS 3

typedef enum msg_type{
    INIT = 1,
    LIST = 2,
    TOALL = 3,
    TOONE = 4,
    STOP = 5
} msg_type;

typedef struct msg_buffer
{
    long mtype;
    int from;
    int to;
    char mtext[MAX_USR_INPUT];
    struct tm time_struct;
} msg_buffer;
