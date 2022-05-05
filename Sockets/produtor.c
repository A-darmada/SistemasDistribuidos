#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Winsock2.h>
#include <unistd.h>
#include <ws2tcpip.h>
#define MSGLEN 256


int main(int argc, char *argv[])
{
    srand(1);
    char message[MSGLEN];
    int rnum = 11;
    int portno = atoi(argv[1]);
    int count = atoi(argv[2]);
    int sockdes, newsockdes, port, prime, n, err;
    socklen_t clilen;
    struct sockaddr_in servaddr, cliaddr;

    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }

    sockdes = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
        0, WSAGetLastError(), 0, message, 256, 0);

    printf("Socket result: %s", message);
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(portno);

    bind(sockdes, (struct sockaddr *) &servaddr,
                sizeof(servaddr));
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
        0, WSAGetLastError(), 0, message, 256, 0);

    printf("Bind result: %s", message);

    listen(sockdes,2);
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
        0, WSAGetLastError(), 0, message, 256, 0);
    printf("Listen result: %s", message);
    
    clilen = sizeof(cliaddr);
    newsockdes = accept(sockdes, 
                 (struct sockaddr *) &cliaddr, &clilen);
        FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
        0, WSAGetLastError(), 0, message, 256, 0);
    printf("New socket result: %s", message);
    
    while(count > 0){
        count--;
        memset(message, 0,sizeof(message));
        int delta = 1 + (rand() % 100);
        rnum += delta;
        printf("rnum is %d\n", rnum);
        sprintf(message, "%d", rnum);

        n = send(newsockdes, message, sizeof(message),MSG_OOB);
        perror("Sending to consumer status: ");

        n = recv(newsockdes, message, sizeof(message)-1,MSG_WAITALL);
        perror("Receiving from consumer status: ");

        prime = atoi(message);
        if (prime == 1){
            printf("%s\n", "Is prime");
            
        }
        if (prime == 0){
            printf("%s\n", "Isn't prime");
            
        }

    }

    closesocket(newsockdes);
    closesocket(sockdes);
}