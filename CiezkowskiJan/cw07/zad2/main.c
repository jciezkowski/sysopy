#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>

#define MAX_NO_LOBBY_SEATS 10

int seg_fd;
sem_t* queue_sem;
sem_t* chair_sem;
sem_t* customer_sem;

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
    sem_close(queue_sem);
    sem_close(chair_sem);
    sem_close(customer_sem);
    sem_unlink("queue");
    sem_unlink("chairs");
    sem_unlink("customers");
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
    queue* queue_ptr;
    int time;
    int barber_id = getpid();

    while(1) {
        printf("numer fryzjera rozpoczynającego pracę: %d\n", barber_id);
        // fryzjer czeka na wolny fotel
        sem_wait(chair_sem);
        // fryzjer czeka na klienta
        sem_wait(customer_sem);
        // oczekiwanie na mozliwosc dostepu do pamieci wspolnej
        sem_wait(queue_sem);
        queue_ptr = mmap(NULL, sizeof(queue), PROT_READ | PROT_WRITE, MAP_SHARED, seg_fd, 0);
        time = queue_ptr->customers[0];
        queue_ptr->next_index--;
        printf("fryzjer nr %d obsługuje klienta\n", barber_id);
        for (int i = 0; i < queue_ptr->next_index; i++) queue_ptr->customers[i] = queue_ptr->customers[i + 1];
        sem_post(queue_sem);
        munmap(queue_ptr, sizeof(queue));
        sleep(time);
        sem_post(chair_sem);
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
    seg_fd = shm_open("/main", O_CREAT | O_RDWR, 0660);
    ftruncate(seg_fd, sizeof(queue));
    queue* queue_ptr = mmap(NULL, sizeof(queue), PROT_READ | PROT_WRITE, MAP_SHARED, seg_fd, 0);
    for (int i = 0; i < no_lobby_seats; i++) queue_ptr->customers[i] = -1;
    queue_ptr->next_index = 0;
    munmap(queue_ptr, sizeof(queue));
    queue_sem = sem_open("queue", O_CREAT | O_RDWR, 0660, 1);
    chair_sem = sem_open("chairs", O_CREAT | O_RDWR, 0660, no_chairs);
    customer_sem = sem_open("customers", O_CREAT | O_RDWR, 0660, 0);
    for (int i = 0; i < no_barbers; i++) {
        pid_t child = fork();
        if (child == 0) {
            customer_handler();
        }
    }
    int time;
    while(1) {
        printf("Przyszedł klient\n");
        time = haircuts[rand()%10];
        sem_wait(queue_sem);
        queue_ptr = mmap(NULL, sizeof(queue), PROT_READ | PROT_WRITE, MAP_SHARED, seg_fd, 0);
        if (queue_ptr->next_index < no_lobby_seats) {
            queue_ptr->customers[queue_ptr->next_index] = time;
            queue_ptr->next_index++;
            sem_post(customer_sem);
        }
        else printf("nie ma miejsca w poczekalni, klient wychodzi\n");
        printf("liczba klientów w poczekalni: %d\nliczba wolnych foteli: %d\n", sem_getvalue(customer_sem, NULL), sem_getvalue(chair_sem, NULL));
        sem_post(queue_sem);
        munmap(queue_ptr, sizeof(queue));
        sleep(rand() % 6 + 2);
    }
}