#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "metrorec.c"

void *passageiros_thread(void *arg) {
    struct estacao *estacao = (struct estacao *)arg;
    estacao_espera_pelo_vagao(estacao); // acho que tá motivando mudança de contexto.
    return NULL;
}

struct vagao_args {
    struct estacao *estacao;
    int assentos_livres;
};

void *vagao_thread(void *args) {
    struct vagao_args *vargs = (struct vagao_args *)args;
    estacao_preecher_vagao(vargs->estacao, vargs->assentos_livres);
    printf("Vagão preenchido\n");
    return NULL;
}

void run_test(int num_passageiros, int num_assentos) {
    struct estacao estacao;
    estacao_init(&estacao);

    //int num_passageiros = 11;
    pthread_t passageiros[num_passageiros];

    for (int i = 0; i < num_passageiros; i++) {
        pthread_create(&passageiros[i], NULL, passageiros_thread, &estacao);
        printf("Passageiro %d criado\n", i+1);
    }

    //int num_assentos = 3;
    int num_passageiros_embarcados = 0;
    int cont = 0;

    while (num_passageiros_embarcados < num_passageiros) {
        struct vagao_args vargs;
        vargs.estacao = &estacao;
        vargs.assentos_livres = num_assentos;

        pthread_t vagao;
        pthread_create(&vagao, NULL, vagao_thread, &vargs);
        printf("Vagão criado com %d assentos livres\n", num_assentos);
        cont++;

        int num_passageiros_a_reap = num_passageiros - num_passageiros_embarcados;
        if (num_passageiros_a_reap > num_assentos) {
            num_passageiros_a_reap = num_assentos;
        }

        for (int i = 0; i < num_passageiros_a_reap; i++) {
            estacao_embarque(&estacao);
            num_passageiros_embarcados++;
            printf("Passageiro embarcou no vagão\n");
        }

        // Verifique se a capacidade do vagão nunca excede o limite máximo
        assert(num_assentos >= num_passageiros_a_reap);
        printf("Todos os passageiros embarcados no vagão\n");
    }

    // Verifique se o número total de passageiros que embarcam nos vagões é igual ao número de passageiros criados
    assert(num_passageiros_embarcados == num_passageiros);

    printf("total de vagoes: %d\n", cont);
    printf("Todos os passageiros foram transportados\n");

    // Adicione asserts para validar se os testes estão funcionando conforme o esperado
    if (num_passageiros == 5 && num_assentos == 5) {
        assert(cont == 1);
    } else if (num_passageiros == 10 && num_assentos == 3) {
        assert(cont == 4);
    } else if (num_passageiros == 6 && num_assentos == 4) {
        assert(cont == 2);
    } else if (num_passageiros == 100 && num_assentos == 25) {
        assert(cont == 4);
    }

// Aguarde todas as threads de passageiros finalizarem
    for (int i = 0; i < num_passageiros; i++) {
        pthread_join(passageiros[i], NULL);
    }
}

int main(void) {
    // Execute a função 'run_test' com diferentes valores para os testes
    run_test(5, 5); // - não passou
    printf("\n\n Proximo: 2° Caso de Teste \n");
    run_test(10, 3); // - passou
    printf("\n\n Proximo: 3° Caso de Teste \n");    
    run_test(6, 4); //- passou
    printf("\n\n Proximo: 4° Caso de Teste \n");
    run_test(100, 25); // passou
    printf("\n\n Ultimo Caso de Teste \n");


    return 0;
}