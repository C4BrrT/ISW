#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct estacao {
    int passageiros;
    int embarcados;
    int livres;
    pthread_mutex_t mutex;
    pthread_cond_t cond_p;
    pthread_cond_t cond_v;
};

void estacao_init(struct estacao *estacao);
void estacao_preecher_vagao(struct estacao * estacao, int assentos);
void estacao_espera_pelo_vagao(struct estacao * estacao);
void estacao_embarque(struct estacao * estacao);

void estacao_init(struct estacao *estacao) {
    estacao->passageiros = 0;
    estacao->embarcados = 0;
    estacao->livres = 0;
    pthread_mutex_init(&estacao->mutex, NULL);
    pthread_cond_init(&estacao->cond_p, NULL);
    pthread_cond_init(&estacao->cond_v, NULL);
}

void estacao_preecher_vagao(struct estacao * estacao, int assentos) {
    pthread_mutex_lock(&estacao->mutex);
    pthread_cond_broadcast(&estacao->cond_p);
    while(assentos > 0){
        estacao->passageiros--;
        assentos--;
    }
    printf("Vagao preenchido\n");
    pthread_mutex_unlock(&estacao->mutex);
}

void estacao_espera_pelo_vagao(struct estacao * estacao) {
    pthread_mutex_lock(&estacao->mutex);
    estacao->passageiros++;
    pthread_cond_wait(&estacao->cond_p, &estacao->mutex);
    /*while(estacao->passageiros > 0){
        estacao->passageiros = pthread_cond_wait(&estacao->cond, &estacao->mutex);
    }*/
    printf("Passageiro esperando pelo vagao\n");
    pthread_mutex_unlock(&estacao->mutex);
}

void estacao_embarque(struct estacao * estacao) {
    pthread_mutex_lock(&estacao->mutex);
    pthread_cond_signal(&estacao->cond_v);
    printf("Passageiro embarcando\n");
    pthread_mutex_unlock(&estacao->mutex);
}