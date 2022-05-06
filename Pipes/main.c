#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>


//SISTEMAS DISTRIBUÍDOS 2022.1 - ALVARO DARMADA E HUSSEIN LATIF

int isPrime(int n) {
	int i; 
	for (i = 2; i <= n / 2; ++i) 
	{
		if (n % i == 0)
            return 0;
        }
	return 1;
}

int main(int argc, char *argv[]) {

	//Criação do pipe para comunicação:
	int anon_pipe[2];
	if (pipe(anon_pipe) < 0){ 
		printf("Erro na criação do pipe.");
	}

    //Cria o processo consumidor através da chamada fork().
    int pid_consumer = fork();

    //Controle de erros:
    if (pid_consumer < 0) {
 		printf ("Falha ao executar comando fork().\n");
		return 1;
    };

    //Trecho de código executado pelo processo consumidor:
    if (pid_consumer==0) {
		//Armazenamento dos números recebidos e declara o buffer de leitura:
		int* number_received = malloc(sizeof(int*));
		*number_received = 1;
		int* read_buffer = malloc(sizeof(int*));

		//O consumidor fecha a ponta de escrita do pipe:
		close(anon_pipe[1]);

		//Realiza a leitura através do pipe:
		while(*number_received != 0){
			read(anon_pipe[0], read_buffer, sizeof(int*));
			*number_received = *read_buffer;

			//Checa e printa se o número recebido é primo ou não:
			if (isPrime(*number_received)){
				printf("O número %i foi recebido e é primo!\n", *number_received);
			} else {
				printf("O número %i foi recebido e NÃO é primo!\n", *number_received);
			};
		};

		//Finaliza a execução:
		printf("O número 0 foi recebido. Consumidor finalizando execução...\n");
		return 0;

    } else {
		//Trecho de código executado pelo processo produtor:
		int* random_number = malloc(sizeof(int*));
		*random_number = 1;
		srand(time(NULL));

		//O produtor fecha a ponta de leitura do pipe:
		close(anon_pipe[0]);

		//O produtor escreve no pipe e gera um novo número aleatório, até que o número de iterações especificado seja atingido.
		for(int i = 0; i < atoi(argv[1]); i++){
			write(anon_pipe[1], random_number, sizeof(int*));
			*random_number += (1 + rand()%100 );
		};

		//Finaliza a execução e aguarda o retorno do consumidor:
		int* eof = malloc(sizeof(int*));
		*eof = 0;
		write(anon_pipe[1], eof, sizeof(int*));
		wait(NULL);
		return 0;
    };
};
