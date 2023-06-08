#include "header.h"

char* sun_path;
int local_d, web_d, epoll_d;
client clients[MAX_NO_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
FILE* logs;

int start_local(char* sun_path) {
    int socket_d = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un sockaddr_struct;
    sockaddr_struct.sun_family = AF_UNIX;
    strcpy(sockaddr_struct.sun_path, sun_path);
    if (bind(socket_d, (struct sockaddr*) &sockaddr_struct, sizeof(sockaddr_struct)) == -1) {
        fprintf(stderr, "bind error\n");
        exit(1);
    }
    listen(socket_d, MAX_NO_CLIENTS);
    return socket_d;
}

int start_web(in_port_t sin_port) {
    int socket_d = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sockaddr_struct;
    sockaddr_struct.sin_family = AF_INET;
    sockaddr_struct.sin_port = sin_port;
    struct in_addr sin_addr;
    sin_addr.s_addr = htons(INADDR_ANY);
    sockaddr_struct.sin_addr = sin_addr;
    if (bind(socket_d, (struct sockaddr*) &sockaddr_struct, sizeof(sockaddr_struct)) == -1) {
        fprintf(stderr, "bind error\n");
    }
    listen(socket_d, MAX_NO_CLIENTS);
    return socket_d;
}

void epoll_add(int fd, int efd) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLPRI;
    ev.data.fd = fd;
    epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev);
}

void message_handler(msg_buffer* msg, client* client) {
    msg_type type = msg->type;
    msg_buffer new_msg;
    char new_log[200];
    int rfd = -1;
    switch(type) {
        case LIST:
            printf("%s: listing\n", msg->from);
            strcpy(new_msg.mtext, "LIST OF ACTIVE USERS:\n");
            for (int i = 0; i < MAX_NO_CLIENTS; i++) {
                if (clients[i].is_active == 1 || clients[i].is_active == 0) {
                    strcat(new_msg.mtext, clients[i].name);
                    strcat(new_msg.mtext, "\n");
                }
            }
            sprintf(new_log, "TIME: %d:%d:%d\nACTION: LIST\nFROM: %s\n\n", msg->time_struct.tm_hour, msg->time_struct.tm_min, msg->time_struct.tm_sec, msg->from);
            fwrite(new_log, sizeof(char), strchr(new_log, '\0') - new_log, logs);
            new_msg.type = LIST;
            write(client->fd, &new_msg, sizeof(new_msg));
            break;
        case TOALL:
            printf("%s: sending to all\n", msg->from);
            strcpy(new_msg.from, msg->from);
            strcpy(new_msg.mtext, msg->mtext);
            new_msg.type = TOALL;
            for (int i = 0; i < MAX_NO_CLIENTS; i++) {
                if (clients[i].is_active == 1) write(clients[i].fd, &new_msg, sizeof(new_msg));
            }
            sprintf(new_log, "TIME: %d:%d:%d\nACTION: MESSAGE TO ALL\nFROM: %s\n\n", msg->time_struct.tm_hour, msg->time_struct.tm_min, msg->time_struct.tm_sec, msg->from);
            fwrite(new_log, sizeof(char), strchr(new_log, '\0') - new_log, logs);
            break;
        case TOONE:
            for (int i = 0; i < MAX_NO_CLIENTS; i++) {
                if ((clients[i].is_active == 1 || clients[i].is_active == 0) && (strcmp(clients[i].name, msg->to) == 0)) {
                    rfd = clients[i].fd;
                    break;
                }
            }
            if (rfd == -1) {
                new_msg.type = ERROR;
                write(client->fd, &new_msg, sizeof(new_msg));
                break;
            }
            printf("%s: sending to one\n", msg->from);
            strcpy(new_msg.from, msg->from);
            strcpy(new_msg.to, msg->to);
            strcpy(new_msg.mtext, msg->mtext);
            new_msg.type = TOONE;
            write(rfd, &new_msg, sizeof(new_msg));
            sprintf(new_log, "TIME: %d:%d:%d\nACTION: MESSAGE TO ONE\nFROM: %s\n\n", msg->time_struct.tm_hour, msg->time_struct.tm_min, msg->time_struct.tm_sec, msg->from);
            fwrite(new_log, sizeof(char), strchr(new_log, '\0') - new_log, logs);
            break;
        case STOP:
            printf("%s: stopping\n", msg->from);
            client->is_active = -1;
            close(client->fd);
            sprintf(new_log, "TIME: %d:%d:%d\nACTION: STOP\nFROM: %s\n\n", msg->time_struct.tm_hour, msg->time_struct.tm_min, msg->time_struct.tm_sec, msg->from);
            fwrite(new_log, sizeof(char), strchr(new_log, '\0') - new_log, logs);
            break;
        case PING:
            client->is_active = 1;
            break;
        default:
            break;
    }
}

void signal_handler(int signo) {
    msg_buffer msg;
    msg.type = STOP;
    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        if (clients[i].is_active != -1) {
            write(clients[i].fd, &msg, sizeof(msg));
            close(clients[i].fd);
        }
    }
    pthread_mutex_destroy(&mutex);
    unlink(sun_path);
    close(local_d);
    close(web_d);
    close(epoll_d);
    exit(0);
}

void* ping(void* args) {
    msg_buffer msg;
    msg.type = PING;
    while(1) {
        sleep(15);
        printf("Pinging\n");
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_NO_CLIENTS; i++) {
            if (clients[i].is_active == -1) continue;
            else if (clients[i].is_active == 0) {
                clients[i].is_active = -1;
                epoll_ctl(epoll_d, EPOLL_CTL_DEL, clients[i].fd, NULL);
                close(clients[i].fd);
            }
            else if (clients[i].is_active == 1){
                clients[i].is_active = 0;
                write(clients[i].fd, &msg, sizeof(msg));
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int check_name_validity(char* name) {
    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        if ((clients[i].is_active == 1 || clients[i].is_active == 0) && strcmp(clients[i].name, name) == 0) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "invalid number of arguments\n");
        return 1;
    }
    in_port_t sin_port = atoi(argv[1]);
    if (strlen(argv[2]) >= UNIX_PATH_MAX) {
        fprintf(stderr, "invalid path length\n");
        return 2;
    }
    logs = fopen("logs.txt", "w");
    sun_path = argv[2];
    unlink(sun_path);
    local_d = start_local(sun_path);
    web_d = start_web(sin_port);
    epoll_d = epoll_create1(0);
    epoll_add(local_d, epoll_d);
    epoll_add(web_d, epoll_d);
    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        client client;
        client.is_active = -1;
        clients[i] = client;
    }
    signal(SIGINT, (void*) signal_handler);
    pthread_t pinging_thread;
    pthread_create(&pinging_thread, NULL, (void*) ping, NULL);
    int no_events;
    struct epoll_event events[MAX_NO_EVENTS];
    printf("server starts\n");
    while(1) {
        no_events = epoll_wait(epoll_d, events, MAX_NO_EVENTS, -1);
        for (int i = 0; i < no_events; i++) {
            if (events[i].data.fd == local_d || events[i].data.fd == web_d) {
                int new_fd = accept(events[i].data.fd, NULL, NULL);
                for (int j = 0; j < MAX_NO_CLIENTS; j++) {
                    if (clients[j].is_active == 1) continue;
                    if (clients[j].is_active == -1) {
                        pthread_mutex_lock(&mutex);
                        msg_buffer msg;
                        read(new_fd, &msg, sizeof(msg));
                        if (check_name_validity(msg.mtext)) {
                            strncpy(clients[j].name, msg.mtext, strlen(msg.mtext));
                            printf("new client, %s\n", clients[j].name);
                            epoll_add(new_fd, epoll_d);
                            clients[j].is_active = 1;
                            clients[j].fd = new_fd;
                            msg.type = HELLO;
                            char new_log[200];
                            sprintf(new_log, "TIME: %d:%d:%d\nACTION: SIGN IN\nFROM: %s\n\n", msg.time_struct.tm_hour, msg.time_struct.tm_min, msg.time_struct.tm_sec, clients[j].name);
                            fwrite(new_log, sizeof(char), strchr(new_log, '\0') - new_log, logs);
                            write(new_fd, &msg, sizeof(msg));
                        }
                        else {
                            msg.type = TAKEN;
                            write(new_fd, &msg, sizeof(msg));
                        }
                        pthread_mutex_unlock(&mutex);
                        break;
                    }
                    else if (j == MAX_NO_CLIENTS) {
                        msg_buffer msg;
                        msg.type = FULL;
                        write(new_fd, &msg, sizeof(msg));
                    }
                }
            }
            else {
                int return_fd = events[i].data.fd;
                msg_buffer msg;
                read(return_fd, &msg, sizeof(msg_buffer));
                client* my_client;
                for (int i = 0; i < MAX_NO_CLIENTS; i++) {
                    if ((clients[i].is_active == 1 || clients[i].is_active == 0) && clients[i].fd == return_fd) {
                        my_client = &clients[i];
                    }
                }
                message_handler(&msg, my_client);
            }
        }
    }
}