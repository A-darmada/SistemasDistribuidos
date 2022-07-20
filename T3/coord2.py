import socket
import time
import os
import numpy as np 
from threading import Thread 

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

# criando filas
fifoMessages = []
auxlist = []
hasStarted = False

#função que retorna quantas vezes cada processo foi atendido TODO
def timesProcess():
    a=np.ones((2,32),float)
    for i in range(1,33):
        a[0][i-1]=i
        a[1][i-1]=0
    for j in range(0,len(a)):
        for w in auxlist:
            if w[2]==j+1:
                a[1][j]+=1
    return a 

#Definindo a interface com o usuário
def interface():
    while True:
        userresp=int(input("Escolha uma das opções a seguir, digitando um desses:\n(1) Imprimir fila de pedidos atual.\n(2) Imprimir quantas vezes cada processo foi atendido.\n(3) Encerrar a execução.\n"))
        if userresp==1:
            print(fifoMessages)
        elif userresp==2:
            print(timesProcess())
        else:
            os.abort()

#onde vão chegar as mensagens de REQUEST, que vão para a fila        
def receiveMessages():
    while True:
        
        ######### recebendo mensagem
        received = UDPServerSocket.recvfrom(bufferSize)
        message = received[0].decode("utf-8")
        print(message)
        ##########


        #########Request/Grant
        if message[0]=="1":
            log = open(r'D:\Estudos\Eng\SISTEMAS DISTRIBUIDOS\Trabalho3\SistemasDistribuidos\T3\log.txt', 'a')
            actualTime = time.perf_counter_ns()
            log.write(str(actualTime) + "  Request recebido. ID: " + str(message[2]) + "\n")
            log.close()
            global hasStarted
            if (not(hasStarted)):  ####Grant inicial
                grant(proccessAddress, message[2])
                hasStarted = True
            fifoMessages.append(message[2]) ####Adicionando nas filas

        ########Release
        elif message[0]=="3":
            
            log = open(r'D:\Estudos\Eng\SISTEMAS DISTRIBUIDOS\Trabalho3\SistemasDistribuidos\T3\log.txt', 'a')
            actualTime = time.perf_counter_ns()
            log.write(str(actualTime))
            log.write("  Release Recebido ")
            log.write("  ID: ")
            log.write(fifoMessages[0])
            log.write("\n")
            log.close()
               
            fifoMessages.pop(0)  #####Retirando processo da fila
            if len(fifoMessages) != 0:
                process = fifoMessages[0]
                grant(proccessAddress, process)

def grant(address, id):
        msgGrant = "2|"+ str(id) +"|000000"
        print(msgGrant)
        log = open(r'D:\Estudos\Eng\SISTEMAS DISTRIBUIDOS\Trabalho3\SistemasDistribuidos\T3\log.txt', 'a')
        actualTime = time.perf_counter_ns()
        log.write(str(actualTime))
        log.write("  Grant enviado para o processo ")
        log.write(id)
        log.write("\n")
        log.close()

        bytesToSend = str.encode(msgGrant)
        UDPServerSocket.sendto(bytesToSend, address)
        return 0

coord= Thread(target=receiveMessages)

inter = Thread(target=interface)

coord.start()
inter.start()