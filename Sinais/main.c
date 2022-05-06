#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

//SISTEMAS DISTRIBUÍDOS 2022.1 - ALVARO DARMADA E HUSSEIN LATIF
//Para executar o programa em linha de comando, após compilado, executar conforme:
// ./nome_do_programa [FORMA DE ESPERAR] [NUMERO DO SINAL], portanto:
// ./nome_do_programa (busy OU blocked) (10 OU 12 OU 15)

volatile int busyWait = 1;

//Função que realiza o envio de sinais:
int send_signal(int signal_id, int pid){

	if (kill(pid, 0) != 0){
		printf("ERRO: O PID informado é inválido. O processo não existe.\n");
		return 1;
	};
	
	kill(pid, signal_id);
	return 0;

};

//Função para o tratamento dos sinais recebidos:
void signal_handler(int signal){

	switch(signal){

		case SIGUSR1:;
			printf ("Sinal SIGUSR1 recebido! Algo incrível irá acontecer!\n");
			break;

		case SIGUSR2:;
			printf ("Sinal SIGUSR2 recebido! Iniciando paz mundial...\n");
			break;

		case SIGTERM:;
			//Interrupção da execução:
			printf("Paz mundial alcançada! Finalizando execução...\n");
			busyWait = 0;
			break;

	};

	return;
};

int main(int argc, char *argv[]) {

    //Cria o processo que receberá os sinais através da chamada fork().
    int receiverPid = fork();

    //Tratamento de erros:
    if (receiverPid < 0) {
		printf ("Falha ao executar comando fork().\n");
		return 1;
    };

    //Trecho de código executado pelo processo que receberá os sinais:
    if (receiverPid==0) {
    	printf("O pid do processo que receberá os sinais é: %i.\n", getpid());

		//Signal handling:
        if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
            printf("Não é possível tratar este sinal.\n");
        };
		if (signal(SIGUSR2, signal_handler) == SIG_ERR) {
            printf("Não é possível tratar este sinal.\n");
        };
        if (signal(SIGTERM, signal_handler) == SIG_ERR) {
            printf("Não é possível tratar este sinal.\n");
        };

		//Busy wait:
		if(strcmp(argv[1], "busy") == 0){
			while(busyWait) {
				sleep(1);
			};

		//Blocking wait:
		} else {
			//Aguarda o retorno do processo que enviará o sinal. Ou seja, aguarda (bloqueado) o envio do sinal.
			pause();
		};

		return 0;
		
    } else {
		//Trecho de código executado pelo processo que enviará os sinais:
		sleep(2);
		send_signal(atoi(argv[2]), receiverPid);
		return 0;

    };
};