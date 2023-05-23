#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "metrorec.c"

volatile int threads_completed;
volatile int load_car_returned;

void *passageiros_thread(void *arg) {
    struct estacao *estacao = (struct estacao *)arg;
    estacao_espera_pelo_vagao(estacao);
    __sync_add_and_fetch(&threads_completed, 1);
    return NULL;
}

struct vagao_args {
    struct estacao *estacao;
    int assentos_livres;
};

void *vagao_thread(void *args) {
    struct vagao_args *vargs = (struct vagao_args *)args;
    estacao_preencher_vagao(vargs->estacao, vargs->assentos_livres);
    load_car_returned = 1;
    return NULL;
}

int main(void) {
    struct estacao estacao;
    estacao_init(&estacao);

    int i;
    const int total_passengers = 12;
    for (i = 0; i < total_passengers; i++) {
        pthread_t tid;
        pthread_create(&tid, NULL, passageiros_thread, &estacao);
    }

    for (i = 0; i <= 2; i++) {
        int free_seats = 2*i + 2;
        printf("car entering station with %d free seats\n", free_seats);
        struct vagao_args args = {&estacao, free_seats};

        pthread_t lt_tid;
        pthread_create(&lt_tid, NULL, vagao_thread, &args);

        int threads_to_reap = free_seats;
        int threads_reaped = 0;

        while (threads_reaped < threads_to_reap) {
            if (threads_completed > 0) {
                threads_reaped++;
                estacao_embarque(&estacao);
                __sync_sub_and_fetch(&threads_completed, 1);
            }
        }

        if (threads_completed > 0) {
            printf("car is leaving without %d passengers\n", threads_completed);
        }

        printf("car departed station with %d new passenger(s) ", threads_reaped);
        printf("(expected %d)%s\n", threads_to_reap, (threads_to_reap != threads_reaped) ? " ***" : "");

    }

    return 0;


}