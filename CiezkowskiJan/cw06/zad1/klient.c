#include "header.h"

int s_queue_id;
int received_id = -1;
msg_buffer* msg_buf;
int c_queue_id;
int server_working = 1;

int isnumber(char* argument) {
    int i = 0;
    while (argument[i] != ' ') {
        if (isdigit(argument[i]) == 0) return 0;
        i++;
    }
    return 1;
}

void read_messages() {
    while(msgrcv(c_queue_id, msg_buf, sizeof(msg_buffer), 0, IPC_NOWAIT) > 0) {
        if (msg_buf->mtype == TOONE) {
            printf("TIME: %d:%d:%d\n", msg_buf->time_struct.tm_hour, msg_buf->time_struct.tm_min, msg_buf->time_struct.tm_sec);
            printf("FROM: %d\n", msg_buf->from);
            printf("MESSAGE TYPE: PRIVATE\n");
            printf("MESSAGE:\n\n%s", msg_buf->mtext);
        }
        else if (msg_buf->mtype == TOALL) {
            printf("TIME: %d:%d:%d\n", msg_buf->time_struct.tm_hour, msg_buf->time_struct.tm_min, msg_buf->time_struct.tm_sec);
            printf("FROM: %d\n", msg_buf->from);
            printf("MESSAGE TYPE: PUBLIC\n");
            printf("MESSAGE:\n%s\n", msg_buf->mtext);
        }
    }
}

void msg_handler(int c_from, msg_type type, char* msg, int c_to) {
    if (server_working == 1) {
        time_t c_time;
        c_time = time(NULL);
        msg_buf->time_struct = *localtime(&c_time);
        msg_buf->from = c_from;
        msg_buf->mtype = type;
        strcpy(msg_buf->mtext, msg);
        msg_buf->to = c_to;
        msgsnd(s_queue_id, msg_buf, sizeof(msg_buffer), 0);
    }
    if (type == STOP){
        free(msg_buf);
        msgctl(c_queue_id, IPC_RMID, NULL);
        exit(0);
    }
}

void sig_handler(int signo) {
    printf("\n");
    printf("STOPPING\n");
    msg_handler(received_id, STOP, "", -1);
}
 
int main() {
    signal(SIGINT, sig_handler);
    srand(time(NULL));
    key_t s_queue_key = ftok(getenv("HOME"), SERVER_ID);
    s_queue_id = msgget(s_queue_key, 0);
    key_t c_queue_key = (getenv("HOME"), SERVER_ID) + rand() % INT_MAX + 1;
    c_queue_id = msgget(c_queue_key, IPC_CREAT | 0666);
    msg_buf = malloc(sizeof(msg_buffer));
    msg_handler(c_queue_id, INIT, "", -1);
    msgrcv(c_queue_id, msg_buf, sizeof(msg_buffer), 0, 0);
    received_id = msg_buf->to;
    if (received_id == -1) {
        fprintf(stderr, "max number of clients\n");
        return 1;
    }
    printf("CLIENT ID: %d\n", received_id);
    char command[MAX_USR_INPUT];
    char type[6];
    char message[MAX_USR_INPUT];
    int to_id = -1;
    while(1) {
        if (msgrcv(c_queue_id, msg_buf, sizeof(msg_buffer), STOP, IPC_NOWAIT) > 0) {
            server_working = 0;
            raise(SIGINT);
        }
        fgets(command, sizeof(command), stdin);
        if (msgrcv(c_queue_id, msg_buf, sizeof(msg_buffer), STOP, IPC_NOWAIT) > 0) {
            printf("SERVER IS DOWN\n");
            server_working = 0;
            raise(SIGINT);
        }
        for (int i = 0; i < 5; i++) {
            type[i] = command[i];
        }
        if (strcmp(type, "list\n") == 0) {
            msg_handler(received_id, LIST, message, to_id);
            msgrcv(c_queue_id, msg_buf, sizeof(msg_buffer), LIST, 0);
            printf("%s\n", msg_buf->mtext);
        }
        else if (strcmp(type, "toall") == 0 && command[5] == ' ') {
            strncpy(message, command + 6, sizeof(message));
            msg_handler(received_id, TOALL, message, to_id);
        }
        else if (strcmp(type, "toone") == 0 && command[5] == ' ') {
            char number[2];
            number[0] = command[6];
            number[1] = command[7];
            if (isnumber(number)) {
                char* separator = strchr(command + 6, ' ');
                to_id = atoi(number);
                strncpy(message, separator + 1, sizeof(message));
                msg_handler(received_id, TOONE, message, to_id);
            }
        }
        else if (strcmp(type, "stop\n") == 0) {
            msg_handler(received_id, STOP, message, to_id);
        }
        else if (strcmp(type, "read\n") == 0) read_messages();
        else printf("invalid command\n");
        if (msgrcv(c_queue_id, msg_buf, sizeof(msg_buffer), STOP, IPC_NOWAIT) > 0) {
            server_working = 0;
            raise(SIGINT);
        }
        to_id = -1;
    }
}