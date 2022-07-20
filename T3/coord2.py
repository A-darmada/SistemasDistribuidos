import socket
import time
import os
from threading import Thread, Semaphore

#defindo local IP e portas
localIP     = "localhost"
localPort   = 8080
bufferSize  = 1024

proccessAddress = ("localhost", 8081)

# Create a datagram socket
UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
 
# Bind to address and ip
UDPServerSocket.bind((localIP, localPort))
 
print("UDP server up and listening")

# criando fila
fifoMessages = []

#Estado necessário para realizar o primeiro Grant
hasStarted = False

#Primeira mensagem é o numero de processos
n_processos = UDPServerSocket.recvfrom(bufferSize)
n = int(n_processos[0].decode("utf-8"))
n_atendimentos = [0] * n

#Semáforos são necessários para controlar o acesso à fila e à lista de atendimento
SemFila = Semaphore()
SemAtendimento = Semaphore()

#Definindo a interface com o usuário
def interface():
    while True:
        userresp=int(input("Escolha uma das opções a seguir, digitando um desses:\n(1) Imprimir fila de pedidos atual.\n(2) Imprimir quantas vezes cada processo foi atendido.\n(3) Encerrar a execução.\n"))
        if userresp==1:
            SemFila.acquire()
            print(fifoMessages)
            SemFila.release()
        elif userresp==2:
            SemAtendimento.acquire()
            print(n_atendimentos)
            SemAtendimento.release()
        elif userresp==3:
            os.abort()
        else : print("COMANDO INVALIDO!\n")

#onde vão chegar as mensagens de REQUEST, que vão para a fila        
def receiveMessages():
    while True:
        
        ######### recebendo mensagem
        received = UDPServerSocket.recvfrom(bufferSize)
        message = received[0].decode("utf-8")
        print(message)

        #########Request/Grant inicial
        if message[0]=="1":
            #Escrevendo no log
            log = open(r'C:\Users\acrda\Projetos\ufrj\SistDist\T3\log.txt', 'a')
            actualTime = time.perf_counter_ns()
            log.write(str(actualTime) + "  Request recebido. ID: " + str(message[2]) + "\n")
            log.close()

            ####Grant inicial
            global hasStarted
            if (not(hasStarted)):  
                grant(proccessAddress, message[2])
                hasStarted = True
            
            #Adicionando processo à fila
            SemFila.acquire()
            fifoMessages.append(message[2]) 
            SemFila.release()

        ########Release
        elif message[0]=="3":
            
            #Escrevendo no log
            SemFila.acquire()
            log = open(r'C:\Users\acrda\Projetos\ufrj\SistDist\T3\log.txt', 'a')
            actualTime = time.perf_counter_ns()
            log.write(str(actualTime) + "  Release Recebido ID:" + str(fifoMessages[0]) + "\n")
            log.close()

            #Retirando processo da fila/chamando grant para o próximo 
            fifoMessages.pop(0)  
            if len(fifoMessages) != 0:
                process = fifoMessages[0]
                grant(proccessAddress, process)
            SemFila.release()


###Parte de logs e comunicação do Grant
def grant(address, id):
        msgGrant = "2|"+ str(id) +"|000000"
        print(msgGrant)
        
        #Escrevendo no log
        log = open(r'C:\Users\acrda\Projetos\ufrj\SistDist\T3\log.txt', 'a')
        actualTime = time.perf_counter_ns()
        log.write(str(actualTime) + "  Grant enviado para o processo " + str(id) + "\n")
        log.close()
        
        #Enviando grant
        bytesToSend = str.encode(msgGrant)
        UDPServerSocket.sendto(bytesToSend, address)
        
        #Acessando lista de atendimento e incrementando o valor para o id do processo
        SemAtendimento.acquire()
        n_atendimentos[int(id)] += 1
        SemAtendimento.release()

        return 0


#Definindo threads para coordenador e interface
coord= Thread(target=receiveMessages)
inter = Thread(target=interface)

#Iniciando threads
coord.start()
inter.start()