#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct estacao {
    int passageiros;
    int embarcados;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

void estacao_init(struct estacao *estacao);
void estacao_preecher_vagao(struct estacao * estacao, int assentos);
void estacao_espera_pelo_vagao(struct estacao * estacao);
void estacao_embarque(struct estacao * estacao);

void estacao_init(struct estacao *estacao) {
    pthread_mutex_init(&estacao->mutex, NULL);
    pthread_cond_init(&estacao->cond, NULL);
}

void estacao_preecher_vagao(struct estacao * estacao, int assentos) {
    //int passageiros = estacao->passageiros;
    //int assentos_livres = assentos;
    pthread_mutex_lock(&estacao->mutex);
    pthread_cond_broadcast(&estacao->cond);
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
    pthread_cond_wait(&estacao->cond, &estacao->mutex);
    /*while(estacao->passageiros > 0){
        estacao->passageiros = pthread_cond_wait(&estacao->cond, &estacao->mutex);
    }*/
    printf("Passageiro esperando pelo vagao\n");
    pthread_mutex_unlock(&estacao->mutex);
}

void estacao_embarque(struct estacao * estacao) {
    pthread_mutex_lock(&estacao->mutex);
    pthread_cond_signal(&estacao->cond);
    printf("Passageiro embarcando\n");
    pthread_mutex_unlock(&estacao->mutex);
}