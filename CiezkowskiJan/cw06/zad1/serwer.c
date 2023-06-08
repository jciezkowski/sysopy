#include "header.h"

int queue_ids[MAX_NO_CLIENTS];
int no_queues = 0;
int s_queue_id;
int signal_received = 0;
FILE* logs;

void sig_handler(int signo) {
    printf("\n");
    printf("STOPPING\n");
    signal_received = 1;
}

void init(msg_buffer* msg_buf) {
    int c_queue = msg_buf->from;
    if (no_queues == MAX_NO_CLIENTS) msg_buf->to = -1;
    else {
        queue_ids[no_queues] = c_queue;
        msg_buf->to = no_queues;
        no_queues++;
    }
    msgsnd(c_queue, msg_buf, sizeof(msg_buffer), 0);
    char new_log[200];
    sprintf(new_log, "TIME: %d:%d:%d\nACTION: INIT\nCLIENT ID: %d\n\n", msg_buf->time_struct.tm_hour, msg_buf->time_struct.tm_min, msg_buf->time_struct.tm_sec, no_queues - 1);
    fwrite(new_log, sizeof(char), strchr(new_log, '\0') - new_log, logs);
}

void list(msg_buffer* msg_buf) {
    int c_queue;
    strcpy(msg_buf->mtext, "List of active clients:\n");
    char line[50];
    for (int i = 0; i < no_queues; i++) {
        sprintf(line, "Client %d: %d\n", i, queue_ids[i]);
        strcat(msg_buf->mtext, line);
    }
    c_queue = queue_ids[msg_buf->from];
    msgsnd(c_queue, msg_buf, sizeof(msg_buffer), 0);
    char new_log[200];
    sprintf(new_log, "TIME: %d:%d:%d\nACTION: LIST\nCLIENT ID: %d\n\n", msg_buf->time_struct.tm_hour, msg_buf->time_struct.tm_min, msg_buf->time_struct.tm_sec, msg_buf->from);
    fwrite(new_log, sizeof(char), strchr(new_log, '\0') - new_log, logs);
}

void toall(msg_buffer* msg_buf) {
    int c_queue;
    for (int i = 0; i < no_queues; i++) {
        if (i != msg_buf->from) {
            c_queue = queue_ids[i];
            msgsnd(c_queue, msg_buf, sizeof(msg_buffer), 0);
        }
    }
    char new_log[200];
    sprintf(new_log, "TIME: %d:%d:%d\nACTION: MESSAGE TO ALL\nFROM: %d\n\n", msg_buf->time_struct.tm_hour, msg_buf->time_struct.tm_min, msg_buf->time_struct.tm_sec, msg_buf->from);
    fwrite(new_log, sizeof(char), strchr(new_log, '\0') - new_log, logs);
}

void toone(msg_buffer* msg_buf) {
    if (msg_buf->to <= no_queues) {
        msgsnd(queue_ids[msg_buf->to], msg_buf, sizeof(msg_buffer), 0);
        char new_log[200];
        sprintf(new_log, "TIME: %d:%d:%d\nACTION: MESSAGE TO ONE\nFROM: %d\nTO: %d\n\n", msg_buf->time_struct.tm_hour, msg_buf->time_struct.tm_min, msg_buf->time_struct.tm_sec, msg_buf->from, msg_buf->to);
        fwrite(new_log, sizeof(char), strchr(new_log, '\0') - new_log, logs);
    }
}

void stop(msg_buffer* msg_buf) {
    for (int i = msg_buf->from; i < no_queues - 1; i++) {
        queue_ids[i] = queue_ids[i + 1];
    }
    queue_ids[no_queues - 1] = -1;
    no_queues--;
    char new_log[200];
    sprintf(new_log, "TIME: %d:%d:%d\nACTION: STOP\nCLIENT ID: %d\n\n", msg_buf->time_struct.tm_hour, msg_buf->time_struct.tm_min, msg_buf->time_struct.tm_sec, msg_buf->from);
    fwrite(new_log, sizeof(char), strchr(new_log, '\0') - new_log, logs);
}

int main() {
    printf("SERVER STARTING\n");
    signal(SIGINT, sig_handler);
    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        queue_ids[i] = -1;
    }
    logs = fopen("logs.txt", "w");
    key_t s_queue_key = ftok(getenv("HOME"), SERVER_ID);
    s_queue_id = msgget(s_queue_key, IPC_CREAT | 0666);
    msg_buffer* msg_buf = malloc(sizeof(msg_buffer));
    while (1) {
        if(msgrcv(s_queue_id, msg_buf, sizeof(msg_buffer), STOP, IPC_NOWAIT) < 0) {
            msgrcv(s_queue_id, msg_buf, sizeof(msg_buffer), -6, IPC_NOWAIT);
        }
        switch(msg_buf->mtype) {
            case INIT:
                printf("initializing\n");
                init(msg_buf);
                break;
            case LIST:
                printf("listing\n");
                list(msg_buf);
                break;
            case TOALL:
                printf("sending to all\n");
                toall(msg_buf);
                break;
            case TOONE:
                printf("sending to one\n");
                toone(msg_buf);
                break;
            case STOP:
                printf("stopping\n");
                stop(msg_buf);
                break;
            default: break;
        }
        if (signal_received == 1) {
            msg_buf->mtype = STOP;
            time_t c_time;
            c_time = time(NULL);
            msg_buf->time_struct = *localtime(&c_time);
            for (int i = 0; i < no_queues; i++) {
                msgsnd(queue_ids[i], msg_buf, sizeof(msg_buffer), 0);
                char new_log[200];
                sprintf(new_log, "TIME: %d:%d:%d\nACTION: STOP\nCLIENT ID: %d\n\n", msg_buf->time_struct.tm_hour, msg_buf->time_struct.tm_min, msg_buf->time_struct.tm_sec, i);
                fwrite(new_log, sizeof(char), strchr(new_log, '\0') - new_log, logs);
            }
            free(msg_buf);
            msgctl(s_queue_id, IPC_RMID, NULL);
            fclose(logs);
            break;
        }
        msg_buf->mtype = 0;
    }
}