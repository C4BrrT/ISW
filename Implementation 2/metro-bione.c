#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "metrorec2.c"

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
	estacao_preecher_vagao(vargs->estacao, vargs->assentos_livres);
	return NULL;
}

int main(void)
{
        //
        // create one station
        //
  struct estacao estacao;
 
   //printf("numero de passageiros na criação: %d\n", estacao.passageiros);
 // estacao.passageiros = 0;
  estacao_init(&estacao);

        //
        // create a number of passengers
        // each pa+ssenger is a thread
        //
  int passageiro = 0;
  pthread_t passageiros[10];
  //estacao.passageiros = 0;
  printf("numero de passageiros na criação: %d\n", estacao.passageiros);
  //pthread_create(&passageiros[0], NULL, (void *)passageiros_thread, (void *)&estacao);
  for(int i = 0; i < 10; i++){
    printf("Walter %d chegou\n", i+1);
    pthread_create(&passageiros[i], NULL, (void *)passageiros_thread, (void *)&estacao);
    passageiro++;
  }

  printf("passageiros: %d\n", passageiro);
    

        
        
        //
        // loop to create as many car as necessary to board all passengers
        //
  int assentos = 20;
  struct vagao_args vargs;
  vargs.estacao = &estacao;  
  vargs.assentos_livres = assentos;
  
  while(passageiro > 0){
                //
                // create only one car with a number of free seats
                // this car is associated to a thread
                //
    
    pthread_t vagao;
    pthread_create(&vagao, NULL, (void *)vagao_thread, (void *)&vargs);
    

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
      //printf("\n\n\n%d, %d\n\n\n", reap, counter);
      if(counter > 0){
        passageiro--;
        assentos--;
        reap--;
        __atomic_fetch_add(&counter, -1, __ATOMIC_SEQ_CST);
        while(reap != 0 && counter == 0){ 
          
        }
        estacao_embarque(&estacao);
      }
    }
    pthread_join(vagao, NULL);
  }

    printf("Terminei");
  return 0;
}