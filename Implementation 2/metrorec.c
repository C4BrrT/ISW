#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct estacao {
    int passageiros;
    int passageiros_pas;
    int embarcados;
    int embarcados_emb;
    int assentos_livres;
    pthread_mutex_t mutex;
    pthread_cond_t cond_p;
    pthread_cond_t cond_v;
    pthread_cond_t cond_e_v;
};

void estacao_init(struct estacao *estacao) {
    estacao->passageiros = 0;
    estacao->embarcados = 0;
    estacao->assentos_livres = 0;
    estacao->embarcados_emb = 0;
    estacao->passageiros_pas =0;
    pthread_mutex_init(&estacao->mutex, NULL);
    pthread_cond_init(&estacao->cond_p, NULL);
    pthread_cond_init(&estacao->cond_v, NULL);
    pthread_cond_init(&estacao->cond_e_v, NULL);
}

void estacao_preencher_vagao(struct estacao * estacao, int assentos) {
    pthread_mutex_lock(&estacao->mutex);
    estacao->assentos_livres = assentos;
    pthread_cond_broadcast(&estacao->cond_p);
    while(estacao->assentos_livres != estacao->embarcados_emb && estacao->passageiros_pas != 0){
        pthread_cond_wait(&estacao->cond_v, &estacao->mutex);
    }
    estacao->assentos_livres=0;
    estacao->embarcados=0;
    estacao->embarcados_emb=0;
    pthread_cond_signal(&estacao->cond_e_v);
    pthread_mutex_unlock(&estacao->mutex);
}

void estacao_espera_pelo_vagao(struct estacao * estacao) {
    pthread_mutex_lock(&estacao->mutex);
    estacao->passageiros++;
    estacao->passageiros_pas++;
    while(estacao->assentos_livres == estacao->embarcados || (estacao->embarcados == 0 && estacao->assentos_livres == 0)){
        pthread_cond_wait(&estacao->cond_p, &estacao->mutex);
    }
    estacao->embarcados++;
    estacao->passageiros--;
    pthread_mutex_unlock(&estacao->mutex);
}

void estacao_embarque(struct estacao * estacao) {
    pthread_mutex_lock(&estacao->mutex);
    estacao->embarcados_emb++;
    estacao->passageiros_pas--;
    if(estacao->embarcados_emb == estacao->assentos_livres || estacao->passageiros_pas == 0){
        pthread_cond_signal(&estacao->cond_v);
        pthread_cond_wait(&estacao->cond_e_v, &estacao->mutex);
    }
    pthread_mutex_unlock(&estacao->mutex);
}