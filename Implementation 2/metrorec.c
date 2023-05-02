#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct estacao {
};

void estacao_init(struct estacao *estacao);
void estacao_preecher_vagao(struct estacao * estacao, int assentos);
void estacao_espera_pelo_vagao(struct estacao * estacao);
void estacao_embarque(struct estacao * estacao);

int main(int argc, char *argv[]){

    return 0;
}

void estacao_init(struct estacao *estacao) {
    pthread_mutex_init
    pthread_cond_init
}

void estacao_preecher_vagao(struct estacao * estacao, int assentos) {
    wait
}

void estacao_espera_pelo_vagao(struct estacao * estacao) {
    
}

void estacao_embarque(struct estacao * estacao) {
    pthread_cond_broadcast
}