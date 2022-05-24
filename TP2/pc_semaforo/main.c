#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>

//SISTEMAS DISTRIBUÍDOS 2022.1 - ALVARO DARMADA E HUSSEIN LATIF
//Para executar o programa em linha de comando, após compilado (usar as flags -lrt, -lpthread e -O1), executar conforme:
// ./nome_do_programa [NÚMEROS DE POSIÇÕES EM MEMÓRIA COMPARTILHADA] [NÚMEROS DE THREADS PRODUTORAS] [NÚMEROS DE THREADS CONSUMIDORAS], portanto:
// ./nome_do_programa (32) (1) (1)

//Semáforos contadores para os números de posições vázias/cheias, e semáforo para exclusão mútua:
sem_t empty;
sem_t full;
sem_t mutex;

//Semáforo para impedir main() de finalizar execução antes de suas threads finalizarem:
sem_t main_control;

//Controle do total de números gerados e consumidos. 
int total_produced = 0;
int total_consumed = 0;

//Controle dos loops de consumo e produção:
int production_control = 1;
int consumption_control = 1;

//Memória compartilhada entre as threads:
int *shared_memory;
int shared_mem_positions;

const char* is_prime(int n) {
	int i; 
	for (i = 2; i <= n / 2; ++i) 
	{
		if (n % i == 0)
            return "não é primo!";
        }
	return "é primo!";
};

int find_empty_position() {
	//Busca e retorna a primeira posição livre na memória compartilhada:
	for(int i = 0; i < shared_mem_positions; i++) {
		if(shared_memory[i] == 0)
			return i;
	};
};

int find_occupied_position() {
	//Busca e retorna a primeira posição ocupada na memória compartilhada:
	for(int i = 0; i < shared_mem_positions; i++) {
		if(shared_memory[i] != 0)
			return i;
	};
};

int generate_number() {
	int random_number = 1 + rand() % 10000000;
	return random_number;
};

void produce() {
	total_produced++;
	shared_memory[find_empty_position()] = generate_number();
};

void* producer(void *arg) {
	while(production_control) {
		//A thread aguarda uma posição de memória vazia e a liberação de mutex:
		sem_wait(&empty);
		sem_wait(&mutex);

		if(total_produced < 100000) {
			produce();
		} else { 
			production_control = 0;
		};

		//Libera o mutex e sinaliza uma posição de memória ocupada:
		sem_post(&mutex);
		sem_post(&full);
	};
};

void consume() {
	total_consumed++;
	int occupied_position = find_occupied_position();
	int number = shared_memory[occupied_position];
	is_prime(number);
	shared_memory[occupied_position] = 0;
};

void* consumer(void *arg){
	while(consumption_control) {
		//A thread aguarda uma posição de memória cheia e a liberação de mutex:
		sem_wait(&full);
		sem_wait(&mutex);

		if(total_consumed < 100000) {
			consume();
		} else { 
			consumption_control = 0;
		};

		//Libera o mutex e sinaliza uma posição de memória livre:
		sem_post(&mutex);
		sem_post(&empty);
	};
	//Libera a execução da main() ao finalizar o consumo:
	sem_post(&main_control);
};

int main(int argc, char *argv[]) {

	//Seed para gerar os números aleatórios:
	srand(time(NULL));

	//Recebe os argumentos de número de threads e posições de memória passados na execução:
	shared_mem_positions = atoi(argv[1]);
	int producer_threads = atoi(argv[2]);
	int consumer_threads = atoi(argv[3]);

	//Inicializa os semáforos:
	sem_init(&empty, 0, shared_mem_positions);
	sem_init(&full, 0, 0);
	sem_init(&mutex, 0, 1);
	sem_init(&main_control, 0, 0);

	//Inicia a contagem do tempo de execução:
	clock_t t; 
	t = clock();

	//Variável adicional necessária para pthread_create:
	pthread_t tid;

	//Alocação e preenchimento inicial (vazio) da memória compartilhada:
	shared_memory = malloc(shared_mem_positions*sizeof(int));
	for (int i = 0; i < shared_mem_positions; i++) {
		shared_memory[i] = 0;
	};

	//Inicialização das threads produtoras e consumidoras:
	for (int i = 0; i < producer_threads; i++) {
		pthread_create(&tid,NULL,producer,NULL);
	};
	for (int i = 0; i < consumer_threads; i++) {
		pthread_create(&tid,NULL,consumer,NULL);
	};

	//Impede que main() finalize antes que todas as threads consumidoras tenham consumido e finalizado:
	sem_wait(&main_control);

	//Finaliza a contagem do tempo de execução:
	t = clock() - t;
	double elapsed_seconds = ((double) t)/CLOCKS_PER_SEC;
	printf("Consumidos: %d || Produzidos: %d \n", total_consumed, total_produced);
	printf("Tempo decorrido: %f\n", elapsed_seconds);

	char buffer[20];
	sprintf(buffer,"%i_%i_%i.csv\0",shared_mem_positions, producer_threads, consumer_threads);
	FILE *fptr;
	fptr = fopen(buffer,"a");
	fprintf(fptr,"%f\n", elapsed_seconds);
	fclose(fptr);

	exit(0);
};
