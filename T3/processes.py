import socket
import time
from threading import Thread


coordinatorAddress = ("localhost", 8080)

bufferSize = 512

UDPSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

n = int(input("Digite o número de processos a serem inicializados: "))
r = int(input("Digite o número de 1execuções de cada processo: "))
k = int(input("Digite o número de segundos que cada processo deve aguardar ao finalizar a região crítica: "))

def requestAccessAndSend(pid):
    requestMessage = "1|" + str(pid) + "|000000"
    encodedMessage = str.encode(requestMessage)

    for i in range(r):

        UDPSocket.sendto(encodedMessage, coordinatorAddress)

        response = UDPSocket.recvfrom(bufferSize)

        if (response[0].decode("utf-8")[0] == "2"):
            results = open("resultados.txt", "a")
            now = time.perf_counter_ns()
            results.write(str(pid) + " - " + str(now))
            results.close()

            releaseMessage = "3|" + str(pid) + "|000000"
            encodedMessage = str.encode(releaseMessage)
            time.sleep(k)
            UDPSocket.sendto(encodedMessage, coordinatorAddress)

threads = list()

for i in range(n):
    x = Thread(target = requestAccessAndSend, args = (i,))
    threads.append(x)
    x.start()