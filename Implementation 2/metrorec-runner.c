#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "metrorec.c"

int counter = 0;

void *passageiros_thread(void *arg)
{
  struct estacao *estacao = (struct estacao *) arg;
	estacao_espera_pelo_vagao(estacao);

   __atomic_fetch_add(&counter, 1, __ATOMIC_SEQ_CST);
  
	return NULL;
}

struct vagao_args {
	struct estacao *estacao;
	int assentos_livres;
};

void *vagao_thread(void *args)
{
	struct vagao_args *vargs = (struct vagao_args *) args;
	estacao_preencher_vagao(vargs->estacao, vargs->assentos_livres);
	return NULL;
}

int run_test(int numPassageiros, int numAssentos)
{
        //
        // create one station
        //
  struct estacao estacao;
  estacao_init(&estacao);
        //
        // create a number of passengers
        // each pa+ssenger is a thread
        //
  int passageiro = 0;
  pthread_t passageiros[numPassageiros];

  for(int i = 0; i < numPassageiros; i++){
    //printf("Passageiro %d chegou\n", i+1);
    pthread_create(&passageiros[i], NULL, (void *)passageiros_thread, (void *)&estacao);
    passageiro++;
  }
  //sleep(2);
        //
        // loop to create as many car as necessary to board all passengers
        //
  struct vagao_args vargs;
  vargs.estacao = &estacao;  
  vargs.assentos_livres = numAssentos;

  //int repCount = 0;
  
  while(passageiro > 0){
    int assentos = numAssentos;
                //
                // create only one car with a number of free seats
                // this car is associated to a thread
                //
    pthread_t vagao;
    pthread_create(&vagao, NULL, (void *)vagao_thread, (void *)&vargs);
    //printf("Vagão %d chegou na estação\n", repCount++);
                //
                // define the number of passenger to reap
                // minimum between number of free seats and passengers at station still waiting to board
                //
    int reap;
    if(passageiro < assentos){
      reap = passageiro;
    }else {
      reap = assentos;
    }
                //
                // for each thread associated to a passenger that finished
                // call estacao_embarque function to let the car know that the passenger is on board
                // ATTENTION: the car can not have more passengers than the number of free seats
                //
    while(reap != 0){
      if(counter > 0){
        passageiro--;
        assentos--;
        reap--;
        __atomic_fetch_add(&counter, -1, __ATOMIC_SEQ_CST);
        //while(reap != 0 && counter == 0);
        estacao_embarque(&estacao);
      }
    }
    if(counter > reap){
      printf("Deu ruim\n");
      exit(0);
    }
    //pthread_join(vagao, NULL);
    printf("Vagão saiu da estação\n");
    }
    printf("Estação finalizada\n");
  return 0;
}

int main(void){
  run_test(5, 5);
  printf("Fim do primeiro teste\n\n");
  run_test(10, 5);
  printf("Fim do segundo teste\n\n");
  run_test(10, 3);
  printf("Fim do terceiro teste\n\n");
  run_test(10, 10);
  printf("Fim do quarto teste\n\n");
  run_test(5, 10);
  printf("Fim do quinto teste\n\n");
  run_test(0, 10);
  printf("Fim do sexto teste\n\n");

}