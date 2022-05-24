#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>


int *arr;
int step;
int sum = 0;
size_t arr_size;


int test_and_set(int *flag) {
    int anterior = *flag;
    *flag = 1;
    return anterior;
}


typedef struct __lock_t {
    int flag;
    } lock_t;

void init(lock_t *lock) {
    lock->flag = 0;
    }

void lock(lock_t *lock) {
    while (test_and_set(&lock->flag)); // spin-wait
    }

void unlock(lock_t *lock) {
    lock->flag = 0;
    }   


lock_t loc; // = {.flag = 0};

// bloco de funcionamento das threads
void * tsoma(void * arg){
    int i;
    int index = *(int*)arg;
    int part = 0;
    int to_sum;
    for (i=index*step; i < ((index+1)*step); i++){
        part += arr[i];
    }
    lock(&loc);
    //Consulta de valor e realização da soma em etapas diferentes
    //para realizar teste de race condition
    to_sum = sum;
    //sleep(1)  //sleep para forçar troca de contexto e garantir race condition caso precise testar
    to_sum += part;
    sum = to_sum;
    unlock(&loc);
}

int main(int argc, char *argv[]){
    //Criando array e pegando argumentos
    int N = atoi(argv[1]);
    int K = atoi(argv[2]);
    int sum_test = 0;
    step = N/K;
    arr_size = N;
    arr = malloc( sizeof(int)*arr_size);
    int i;
    srand(0);
    //gerando números randoms
    for (i=0; i < N; i++){
        arr[i] = (rand()%201) - 100;
    }

    struct timeval finish, start;
    //Criando threads
    mingw_gettimeofday(&start, NULL);
    pthread_t tid[K];
    for (i = 0; i < K; i++) {
        int* ind = malloc(sizeof(int)); 
        //os "ids" das threads foram pré-definidos para garantir ordenação 
        *ind = i;
		pthread_create(&tid[i],NULL,tsoma,ind);
	};
    
    //So continua o código quando todas as threads acabarem,
    //evitando que o main acabe antes das threads
    for (i = 0; i < K; i++) {
		pthread_join(tid[i],NULL);
	};


    mingw_gettimeofday(&finish, NULL);
    free(arr);
    double secs = (double)(finish.tv_usec - start.tv_usec) / 1000000 + (double)(finish.tv_sec - start.tv_sec);
    printf("%f",secs); 
    return 0;
}