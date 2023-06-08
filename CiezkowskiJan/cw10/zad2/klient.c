#include "header.h"

int fd;
char* client_name;

struct sockaddr_un connect_local(const char* path, sa_family_t family) {
    fd = socket(family, SOCK_STREAM, 0);
    struct sockaddr_un sockaddr_struct;
    sockaddr_struct.sun_family = family;
    strcpy(sockaddr_struct.sun_path, path);
    connect(fd, (struct sockaddr*) &sockaddr_struct, sizeof(sockaddr_struct));
    return sockaddr_struct;
}

struct sockaddr_in connect_web(const char* ip, in_port_t port, sa_family_t family) {
    fd = socket(family, SOCK_STREAM, 0);
    struct sockaddr_in sockaddr_struct;
    sockaddr_struct.sin_family = family;
    sockaddr_struct.sin_port = htons(port);
    connect(fd, (struct sockaddr*) &sockaddr_struct, sizeof(sockaddr_struct));
    return sockaddr_struct;
}

void signal_handler(int signo) {
    msg_buffer msg;
    msg.type = STOP;
    time_t c_time;
    c_time = time(NULL);
    msg.time_struct = *localtime(&c_time);
    strcpy(msg.from, client_name);
    write(fd, &msg, sizeof(msg));
    close(fd);
    exit(0);
}

int main(int argc, char** argv) {
    if (argc != 4 && argc != 5) {
        fprintf(stderr, "invalid number of arguments\n");
        return 1;
    }
    if (strlen(argv[1]) > MAX_CLIENT_NAME_LEN) {
        fprintf(stderr, "invalid name length\n");
        return 2;
    }
    client_name = argv[1];
    sa_family_t sin_family;
    if (strcmp(argv[2], "local") == 0) sin_family = AF_UNIX;
    else if (strcmp(argv[2], "web") == 0) sin_family = AF_INET;
    else {
        fprintf(stderr, "invalid connection mode\n");
        return 3;
    }
    if (strlen(argv[3]) >= UNIX_PATH_MAX) {
        fprintf(stderr, "invalid address length\n");
        return 4;
    }
    char* address = argv[3];
        msg_buffer msg;
    if (sin_family == AF_UNIX) {
        struct sockaddr_un addr = connect_local(address, sin_family);
        msg.addr.addr_un = addr;
        msg.addrlen = sizeof(addr);
    }
    else {
        if (argc != 5) {
            fprintf(stderr, "invalid number of arguments\n");
            return 5;
        }
        in_port_t port = atoi(argv[4]);
        struct sockaddr_in addr = connect_web(address, port, sin_family);
        msg.addr.addr_in = addr;
        msg.addrlen = sizeof(addr);
    }
    signal(SIGINT, (void*) signal_handler);
    time_t c_time;
    c_time = time(NULL);
    msg.time_struct = *localtime(&c_time);
    strncpy(msg.mtext, client_name, strlen(client_name));
    int epoll_d = epoll_create1(0);
    struct epoll_event stdin_ev;
    stdin_ev.events = EPOLLIN | EPOLLPRI;
    stdin_ev.data.fd = STDIN_FILENO;
    epoll_ctl(epoll_d, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_ev);
    struct epoll_event socket_ev;
    socket_ev.events = EPOLLIN | EPOLLPRI | EPOLLHUP;
    socket_ev.data.fd = fd;
    epoll_ctl(epoll_d, EPOLL_CTL_ADD, fd, &socket_ev);
    send(fd, &msg, sizeof(msg), 0);
    struct epoll_event events[2];
    int no_events;
    char input[MAX_USR_INPUT + 1];
    char type[6];
    while(1) {
        no_events = epoll_wait(epoll_d, events, 2, -1);
        for (int i = 0; i < no_events; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                msg_buffer msg;
                time_t c_time;
                c_time = time(NULL);
                msg.time_struct = *localtime(&c_time);
                strcpy(msg.from, client_name);
                read(STDIN_FILENO, input, MAX_USR_INPUT);
                for (int i = 0; i < 5; i++) {
                    type[i] = input[i];
                }
                if (strcmp(type, "list\n") == 0) msg.type = LIST;
                else if (strcmp(type, "toall") == 0 && input[5] == ' ') {
                    msg.type = TOALL;
                    strncpy(msg.mtext, input + 6, strlen(input + 6) - 1);
                }
                else if (strcmp(type, "toone") == 0 && input[5] == ' ') {
                    msg.type = TOONE;
                    char* separator = strchr(input + 6, ' ');
                    strncpy(msg.to, input + 6, separator - (input + 6));
                    strncpy(msg.mtext, separator + 1, strlen(separator + 1) - 1);
                }
                else if (strcmp(type, "stop\n") == 0) msg.type = STOP;
                else {
                    printf("invalid command\n");
                    continue;
                }
                write(fd, &msg, sizeof(msg));
            }
            else {
                msg_buffer msg;
                time_t c_time;
                c_time = time(NULL);
                msg.time_struct = *localtime(&c_time);
                read(fd, &msg, sizeof(msg));
                if (events[i].events & EPOLLHUP) {
                    printf("Disconnected\n");
                    return 0;
                }
                else {
                    switch(msg.type) {
                        case LIST:
                            printf("%s\n", msg.mtext);
                            break;
                        case TOONE:
                            printf("%s: %s\n", msg.from, msg.mtext);
                            break;
                        case TOALL:
                            printf("%s: %s\n", msg.from, msg.mtext);
                            break;
                        case FULL:
                            printf("Server full\n");
                            close(fd);
                            return 0;
                        case TAKEN:
                            printf("Name taken\n");
                            close(fd);
                            return 0;
                        case STOP:
                            close(fd);
                            return 0;
                        case PING:
                            strcpy(msg.from, client_name);
                            write(fd, &msg, sizeof(msg));
                            break;
                        case HELLO:
                            printf("hello\n");
                            break;
                        case ERROR:
                            printf("invalid username\n");
                            break;
                        default:
                            break;
                    } 
                }
            }
        }
    }
}