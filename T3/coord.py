#!/usr/bin/env python
# coding: utf-8

#imports
import socket
import time
import os
import numpy as np 
from collections import deque
from queue import Queue
from threading import Thread 

#defindo local IP e portas
localIP     = "localhost"
localIP2     = "localhost"
localPort   = 8080
localPort2   = 8081
bufferSize  = 1024



# Create a datagram socket

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
UDPServerSocket2 = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
 

# Bind to address and ip

UDPServerSocket.bind((localIP, localPort))
UDPServerSocket2.bind((localIP2, localPort2))
 

print("UDP server up and listening")

 

# criando filas
fifoMessages = deque([])
addresses = deque([])
auxlist = []

#função que retorna quantas vezes cada processo foi atendido 
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
        bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)
        print(bytesAddressPair)
        message = bytesAddressPair[0]
        address = bytesAddressPair[1]
        log = open(r'C:\Users\acrda\Projetos\ufrj\SistDist\T3\log.txt', 'a')
        actualTime = time.perf_counter_ns()
        time.sleep(1)
        log.write(str(actualTime))
        log.write("  Request recebido. ")
        log.write("ID: ")
        log.write(message.decode("utf-8")[2])
        log.write("\n")
        log.close()
        fifoMessages.append(message.decode("utf-8"))
        addresses.append(address)

#algoritmo centralizado, que fica em loop, verifica o tamanho da fila, e executa o algoritmo.
def central():
    while(True):
        if len(fifoMessages)!=0:
            message = fifoMessages[0]
            address = addresses[0]

            if message[0]=="1":
                msgGrant = "2|"+"3"+"|000000"
                fifoMessages.popleft()
                addresses.popleft()  

            log = open(r'C:\Users\acrda\Projetos\ufrj\SistDist\T3\log.txt', 'a')
            actualTime = time.perf_counter_ns()
            log.write(str(actualTime))
            log.write("  Grant enviado para o processo ")
            log.write(message[2])
            log.write("\n")
            log.close()

            bytesToSend = str.encode(msgGrant)

            # Sending a reply to client - GRANT
            UDPServerSocket.sendto(bytesToSend, address)

            bytesAddressPair2 = UDPServerSocket2.recvfrom(bufferSize)

           
            messager = bytesAddressPair2[0].decode("utf-8")
            auxlist.append(messager)
            log = open(r'C:\Users\acrda\Projetos\ufrj\SistDist\T3\log.txt', 'a')
            actualTime = time.perf_counter_ns()
            log.write(str(actualTime))
            log.write("  Release Recebido ")
            log.write("  ID: ")
            log.write(messager[2])
            log.write("\n")
            log.close()

        else:
            continue

master = Thread(target=central)


aux = Thread(target=receiveMessages)

inter = Thread(target=interface)

master.start()
aux.start()
inter.start()