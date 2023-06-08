#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/shm.h>
#include <unistd.h>

#define MAX_NO_LOBBY_SEATS 10

int sem_id;
int seg_id;


typedef struct queue {
    int customers[MAX_NO_LOBBY_SEATS];
    int next_index;
} queue;

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    struct seminfo* __buf;
};

void sigint_handler(int signo) {
    semctl(sem_id, 0, IPC_RMID);
    semctl(sem_id, 1, IPC_RMID);
    semctl(sem_id, 2, IPC_RMID);
    kill(-1 * getpid(), SIGTERM);
}

void sigterm_handler() {
    printf("proces %d kończy pracę\n", getpid());
    exit(0);
}

void randomize_haircuts(int haircuts[], int length) {
    srand(time(NULL));
    for (int i = 0; i < length; i++) {
        haircuts[i] = rand() % 10 + 10;
    }
}

int is_number(char* arg) {
    for (int i = 0; i < strlen(arg); i++) {
        if (!(isdigit(arg[i]))) {
            return 0;
        }
    }
    return 1;
}

void customer_handler() {
    struct sembuf buffer;
    queue* queue_ptr;
    int time;
    int barber_id = getpid();

    while(1) {
        printf("numer fryzjera rozpoczynającego pracę: %d\n", barber_id);
        // fryzjer czeka na wolny fotel
        buffer.sem_num = 1;
        buffer.sem_op = -1;
        buffer.sem_flg = 0;
        semop(sem_id, &buffer, 1);
        // fryzjer czeka na klienta
        buffer.sem_num = 2;
        buffer.sem_op = -1;
        semop(sem_id, &buffer, 1);
        // oczekiwanie na mozliwosc dostepu do pamieci wspolnej
        buffer.sem_num = 0;
        buffer.sem_op = -1;
        semop(sem_id, &buffer, 1);

        queue_ptr = shmat(seg_id, NULL, 0);
        time = queue_ptr->customers[0];
        queue_ptr->next_index--;
        printf("fryzjer nr %d obsługuje klienta\n", barber_id);
        for (int i = 0; i < queue_ptr->next_index; i++) queue_ptr->customers[i] = queue_ptr->customers[i + 1];
        buffer.sem_num = 0;
        buffer.sem_op = 1;
        semop(sem_id, &buffer, 1);
        shmdt(queue_ptr);
        sleep(time);
        buffer.sem_num = 1;
        buffer.sem_op = 1;
        semop(sem_id, &buffer, 1);
        printf("fryzjer nr %d skończył obsługiwać klienta\n", barber_id);
    }
}

int main(int argc, char** argv) {
    if (argc != 5) {
        fprintf(stderr, "invalid number of arguments\n");
        return 1;
    }
    if (!(is_number(argv[1]) && is_number(argv[2]) && is_number(argv[3]) && is_number(argv[4]))) {
        fprintf(stderr, "arguments have to be integers\n");
        return 2;
    }
    int no_barbers = atoi(argv[1]);
    int no_chairs = atoi(argv[2]);
    int no_lobby_seats = atoi(argv[3]);
    int no_haircuts = atoi(argv[4]);
    int haircuts[no_haircuts];
    randomize_haircuts(haircuts, no_haircuts);
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigterm_handler);
    seg_id = shmget(ftok(getenv("HOME"), 1), sizeof(queue), IPC_CREAT | 0660);
    queue* queue_ptr = shmat(seg_id, NULL, 0);
    for (int i = 0; i < MAX_NO_LOBBY_SEATS; i++) queue_ptr->customers[i] = -1;
    queue_ptr->next_index = 0;
    sem_id = semget(ftok(getenv("HOME"), 1), 3, IPC_CREAT | 0660);
    shmdt(queue_ptr);
    union semun semun;
    semun.val = 1;
    semctl(sem_id, 0, SETVAL, semun);
    semun.val = no_chairs;
    semctl(sem_id, 1, SETVAL, semun);
    semun.val = 0;
    semctl(sem_id, 2, SETVAL, semun);
    for (int i = 0; i < no_barbers; i++) {
        pid_t child = fork();
        if (child == 0) {
            customer_handler();
        }
    }
    int time;
    struct sembuf buffer;
    while(1) {
        printf("Przyszedł klient\n");
        time = haircuts[rand()%10];
        buffer.sem_num = 0;
        buffer.sem_op = -1;
        buffer.sem_flg = 0;
        semop(sem_id, &buffer, 1);
        queue_ptr = shmat(seg_id, NULL, 0);
        if (queue_ptr->next_index < no_lobby_seats) {
            queue_ptr->customers[queue_ptr->next_index] = time;
            queue_ptr->next_index++;
            buffer.sem_num = 2;
            buffer.sem_op = 1;
            semop(sem_id, &buffer, 1);
        }
        else printf("nie ma miejsca w poczekalni, klient wychodzi\n");
        printf("liczba klientów w poczekalni: %d\nliczba wolnych foteli: %d\n", semctl(sem_id, 2, GETVAL), semctl(sem_id, 1, GETVAL));
        buffer.sem_num = 0;
        buffer.sem_op = 1;
        semop(sem_id, &buffer, 1);
        shmdt(queue_ptr);
        sleep(rand() % 6 + 2);
    }
}