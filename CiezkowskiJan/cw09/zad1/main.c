#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elf_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
pthread_t* reindeer_threads;
pthread_t* elf_threads;
int no_reindeers = 0;
int no_elves = 0;
int elves_waiting[3];

typedef struct arguments {
    int index;
} arguments;

void* reindeer_working(void* args) {
    arguments* arg = (arguments*) args;
    int index = arg->index;
    free(args);
    srand(time(NULL));
    while(1) {
        sleep(rand()%6 + 5);
        pthread_mutex_lock(&mutex);
        no_reindeers++;
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", no_reindeers, index);
        if (no_reindeers == 9) {
            printf("Renifer: wybudzam Mikołaja, %d\n", index);
            pthread_cond_broadcast(&santa_cond);
        }
        while (no_reindeers != 0) pthread_cond_wait(&reindeer_cond, &mutex);
        pthread_mutex_unlock(&mutex);
    }
}

void* elf_working(void* args) {
    arguments* arg = (arguments*) args;
    int index = arg->index;
    free(args);
    srand(time(NULL));
    while(1) {
        sleep(rand()%4 + 2);
        pthread_mutex_lock(&mutex);
        if (no_elves < 3) {
            elves_waiting[no_elves] = index;
            no_elves++;
            printf("Elf: czeka %d elfów na Mikołaja, %d\n", no_elves, index);
            if (no_elves == 3) {
                printf("Elf: wybudzam Mikołaja, %d\n", index);
                pthread_cond_broadcast(&santa_cond);
            }
            while (no_elves != 0) pthread_cond_wait(&elf_cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
        else {
            printf("Elf: samodzielnie rozwiązuję swój problem, %d\n", index);
            pthread_mutex_unlock(&mutex);
        }
    }
}

void handler(int signo) {
    free(elf_threads);
    free(reindeer_threads);
    exit(0);
}

int main(void) {
    srand(time(NULL));
    int no_presents_delivered = 0;
    reindeer_threads = malloc(sizeof(pthread_t) * 9);
    elf_threads = malloc(sizeof(pthread_t) * 10);
    signal(SIGINT, handler);
    for (int i = 0; i < 9; i++) {
        arguments* arg = malloc(sizeof(arguments));
        arg->index = i;
        pthread_create(&reindeer_threads[i], NULL, (void*)reindeer_working, (void*)arg);
    }
    for (int i = 0; i < 10; i++) {
        arguments* arg = malloc(sizeof(arguments));
        arg->index = i;
        pthread_create(&elf_threads[i], NULL, (void*)elf_working, (void*)arg);
    }
    while(1) {
        printf("Mikołaj: zasypiam\n");
        pthread_mutex_lock(&mutex);
        while (no_reindeers < 9 && no_elves < 3) {
            pthread_cond_wait(&santa_cond, &mutex);
        }
        printf("Mikołaj: budzę się\n");
        if (no_elves == 3) {
            printf("Mikołaj: rozwiązuje problemy elfów %d, %d, %d\n", elves_waiting[0], elves_waiting[1], elves_waiting[2]);
            pthread_mutex_unlock(&mutex);
            sleep(rand()%2 + 1);
            no_elves = 0;
            pthread_cond_broadcast(&elf_cond);
        }
        if (no_reindeers == 9) {
            printf("Mikołaj: dostarczam zabawki\n");
            pthread_mutex_unlock(&mutex);
            sleep(rand()%3 + 2);
            no_presents_delivered++;
            no_reindeers = 0;
            pthread_cond_broadcast(&reindeer_cond);
        }
        if (no_presents_delivered == 3) {
            printf("Mikołaj: dostarczyłem 3 prezenty\n");
            free(elf_threads);
            free(reindeer_threads);
            break;
        }
    }
    return 0;
}