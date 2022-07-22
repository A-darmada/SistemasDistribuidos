import socket
import time
from threading import Thread


coordinatorAddress = ("localhost", 8080)
localIP     = "localhost"
localPort   = 8081

bufferSize = 1024

UDPSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

UDPSocket.bind((localIP, localPort))

n = int(input("Digite o número de processos a serem inicializados: "))
r = int(input("Digite o número de execuções de cada processo: "))
k = int(input("Digite o número de segundos que cada processo deve aguardar ao finalizar a região crítica: "))

def requestAccessAndSend(pid):
    requestMessage = "1|" + str(pid) + "|000000"
    encodedRequestMessage = str.encode(requestMessage)

    for i in range(r):

        print(requestMessage)
        UDPSocket.sendto(encodedRequestMessage, coordinatorAddress)
        

        response = UDPSocket.recvfrom(bufferSize)
        flags = response[0].decode("utf-8").split("|")

        if (flags[0] == "2" and flags[1] == str(pid)):
            results = open("resultados.txt", "a")
            now = time.perf_counter_ns()
            results.write("Processo: " + str(pid) + " - Tempo: " + str(now) + "\n")
            results.close()

            releaseMessage = "3|" + str(pid) + "|000000"
            print(releaseMessage)
            encodedResponseMessage = str.encode(releaseMessage)
            time.sleep(k)
            UDPSocket.sendto(encodedResponseMessage, coordinatorAddress)
    

threads = list()

open("resultados.txt", "w").close()

UDPSocket.sendto(str.encode(str(n)), coordinatorAddress) #Informando à central o número de processos

for i in range(n):
    x = Thread(target = requestAccessAndSend, args = (i,))
    threads.append(x)
    x.start()
