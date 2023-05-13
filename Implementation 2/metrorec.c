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
void estacao_preencher_vagao(struct estacao * estacao, int assentos);
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

void estacao_preencher_vagao(struct estacao * estacao, int assentos) {
    pthread_mutex_lock(&estacao->mutex);
    //printf("Vagão chegando\n");
    estacao->livres = assentos;
    pthread_cond_broadcast(&estacao->cond_p);
    printf("Faltam %d passageiros embarcarem\n", estacao->passageiros);
    while(estacao->livres > 0 && estacao->passageiros > 0){
        pthread_cond_wait(&estacao->cond_v, &estacao->mutex);
    }
    printf("Vagao preenchido\n");
    pthread_mutex_unlock(&estacao->mutex);
}

void estacao_espera_pelo_vagao(struct estacao * estacao) {
    pthread_mutex_lock(&estacao->mutex);
    estacao->passageiros++;
    printf("Passageiro %d começou a esperar\n", estacao->passageiros);
    while(estacao->livres == 0){
        pthread_cond_wait(&estacao->cond_p, &estacao->mutex);
    }
    printf("Passageiro %d saiu da espera\n", estacao->passageiros);
    estacao->livres--;
    estacao->passageiros--;
    pthread_mutex_unlock(&estacao->mutex);
}

void estacao_embarque(struct estacao * estacao) {
    pthread_mutex_lock(&estacao->mutex);
    estacao->embarcados++;
    printf("Agora tem %d assentos livres no vagão, e %d passageiros embarcaram\n", estacao->livres, estacao->embarcados);
    if(estacao->livres == 0 || estacao->passageiros == 0){
        pthread_cond_signal(&estacao->cond_v);
    }
    pthread_mutex_unlock(&estacao->mutex);
}