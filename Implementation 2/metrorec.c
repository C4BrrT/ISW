#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct estacao {
    int passageiros;
    int embarcados;
    int assentos_livres;
    pthread_mutex_t mutex;
    pthread_cond_t cond_p;
    pthread_cond_t cond_v;
    //pthread_cond_t cond_e;
};

void estacao_init(struct estacao *estacao);
void estacao_preecher_vagao(struct estacao * estacao, int assentos);
void estacao_espera_pelo_vagao(struct estacao * estacao);
void estacao_embarque(struct estacao * estacao);

void estacao_init(struct estacao *estacao) { //1
    estacao->passageiros = 0;
    estacao->embarcados = 0;
    estacao->assentos_livres = 0;
    pthread_mutex_init(&estacao->mutex, NULL);
    pthread_cond_init(&estacao->cond_p, NULL);
    pthread_cond_init(&estacao->cond_v, NULL);
}

void estacao_preecher_vagao(struct estacao * estacao, int assentos) { //4
    pthread_mutex_lock(&estacao->mutex);
    estacao->assentos_livres = assentos;
    pthread_cond_broadcast(&estacao->cond_p);
    pthread_cond_wait(&estacao->cond_v, &estacao->mutex);
    // op1: pthread_cond_wait(&estacao->cond_e, &estacao->mutex);
    if(estacao->assentos_livres==0 || estacao->passageiros==0){
        pthread_cond_signal(&estacao->cond_v);
    }
    /*while(estacao->assentos_livres > 0){
        estacao->passageiros--;
        estacao->assentos_livres--;
        estacao->embarcados++;
    }*/
    //pthread_cond_signal(&estacao->cond_v);
    // op1/2: pthread_cond_signal(&estacao->cond_e);
    pthread_mutex_unlock(&estacao->mutex);
}

void estacao_espera_pelo_vagao(struct estacao * estacao) { //2
    pthread_mutex_lock(&estacao->mutex);
    estacao->passageiros++;
    printf("Passageiro esperando pelo vagao\n");
    printf("%d \n", estacao->passageiros);
    //pthread_cond_wait(&estacao->cond_p, &estacao->mutex);
    while(estacao->assentos_livres == 0){
        printf("teste\n");
        pthread_cond_wait(&estacao->cond_p, &estacao->mutex);
    }
    pthread_mutex_unlock(&estacao->mutex);
}

void estacao_embarque(struct estacao * estacao) { //3
    pthread_mutex_lock(&estacao->mutex);
    while(estacao->assentos_livres > 0){
        estacao->passageiros--;
        estacao->assentos_livres--;
        estacao->embarcados++;
        // op2: pthread_cond_wait(&estacao->cond_e, &estacao->mutex);
    }
    pthread_cond_signal(&estacao->cond_v);
    //op 1/2: pthread_cond_signal(&estacao->cond_e);
    printf("Passageiro embarcando\n");
    printf("%dE \n", estacao->embarcados);
    pthread_mutex_unlock(&estacao->mutex);
}