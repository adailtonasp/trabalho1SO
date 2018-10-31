/*
O codigo abaixo ira funcionar somente em plataformas UNIX

Compilando o codigo: gcc filosofos.c -o filosofos -pthread

Executando: ./filosofos
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

#define n 5 //quantidade de filosfos
#define esquerda (i + n - 1) % n //filosofo da esquerda
#define direita (i + 1) % n //filosofo da direita
#define pensando 0
#define faminto 1
#define comendo 2

int estado[n]; //estado de cada filosofo
sem_t s[n]; //um semaforo para cada filosofo - e inicializado com 0
sem_t mutex; //semaforo para pegar garfo - e inicializado com 1

//acoes do filosofo
void pensar(int i){
	sleep(2);
};
void comer(){
	sleep(2);
}

//funcao que ve se um filosofo esta apto a comer
void test(int i){
	//se o filosofo i esta com fome e os filosofos vizinhos nao estao comendo(!= comendo)
	//o filosofo i pode comer
    if(estado[i] == faminto && estado[esquerda] != comendo && estado[direita] != comendo){
        estado[i] = comendo;
        printf("O filosofo %d esta comendo agora\n\n",i + 1);
        sem_post(&s[i]); //up no mutex do filosofo
    }else{
		if((estado[esquerda] == comendo) && (estado[direita] != comendo))
			printf("O filosofo %d tentou pegar os garfos para comer. Porem o filosofo %d ja estava comendo\n\n",i + 1, esquerda + 1);
		if((estado[esquerda] != comendo) && (estado[direita] == comendo))
			printf("O filosofo %d tentou pegar os garfos para comer. Porem o filosofo %d ja estava comendo\n\n",i + 1, direita + 1);
		if((estado[esquerda] == comendo) && (estado[direita] == comendo))
			printf("O filosofo %d tentou pegar os garfos para comer. Porem ambos os vizinhos ja estavam comendo\n\n", i + 1);
	}
}

//regiao critica para pegar os garfos
void pegar_garfos(int i){
	sem_wait(&mutex); //down no mutex
    estado[i] = faminto; // Filosofo  i faminto
    printf("O filosofo %d esta faminto!\n\n", i + 1);
    test(i); // Filosofo tenta pegar os dois garfos
    sem_post(&mutex); // up no mutex
    sem_wait(&s[i]); // down no mutex do filoso i - bloqueia se os garfos nao foram pegos
}

//regiao critica para devolver os garfos
void devolve_garfos(int i){
	sem_wait(&mutex); //down no mutex
	estado[i] = pensando; //o filosofo agora esta pensando
	printf("O filosofo %d terminou de comer e voltou a pensar\n\n", i + 1);
	test(esquerda); //ve se o vizinho da esquerda pode comer agora */
	test(direita); // ve se o vizinho da direita pode comer agora */
	sem_post(&mutex); //up do mutex
}

void *filosofo(void * i){
	int filo = i;
	while(1){ //loop infinito
		pensar(filo);
		pegar_garfos(filo);
		comer();
		devolve_garfos(filo);
	}
}

int main ( ) {
	int i;

	//inicializar o semaforo
	sem_init(&mutex, 0, 1); //semaforo do mutex
    for(i = 0; i < n; i++)
        sem_init(&s[i], 0, 0); //semaforo de cada filosofo

    //criar os filofosos
    int status; //estado do criado filosofo
	pthread_t filosofos[n];
	for(i = 0; i < n; i++){ //coloca os filosofos em execucao
		if(i == n)
			break;
		status = pthread_create(&filosofos[i],NULL,filosofo,(void *)i); //cria uma thread para cada filosofo
		if(status != 0){
			printf("Nao foi possivel criar a thread. Codigo do erro: %d\n", status);
			exit(-1);
		}
	}
	while(1){
        sem_wait(&mutex);
		printf("--------------------------------------\n");
		for(i = 0; i < n; i++){
			if(estado[i] == 1)
				printf("O filosofo %d esta %s agora\n", i + 1, "faminto");
			else if(estado[i] == 2)
				printf("O filosofo %d esta %s agora\n", i + 1, "comendo");
			else
				printf("O filosofo %d esta %s agora\n", i + 1, "pensando");
		}
		printf("--------------------------------------\n\n");
        sem_post(&mutex);
		sleep(2);
	}
	return 0;
}
