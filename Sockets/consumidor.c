#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Winsock2.h>
#include <unistd.h>
#include <ws2tcpip.h>
#define MSGLEN 256


int main(int argc, char *argv[])
{
    int pnum, prime, err;
    int sockfd, portno, n;
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    
    err = WSAStartup(wVersionRequested, &wsaData);
    char message[MSGLEN];
    
    
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }

    
    struct sockaddr_in serv_addr;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Sockfd is: %d\n", sockfd);
    
    if (sockfd < 0) {
        fprintf(stderr, "Value of errno: %d\n", errno);
    }
    portno = atoi(argv[2]);
    server = gethostbyname(argv[1]);
    
    serv_addr.sin_family = AF_INET;
    memset((char *) &serv_addr, 0,sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;    
    memcpy((char *)&serv_addr.sin_addr.s_addr, 
         (char *)server->h_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    int con = connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)); 
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
        0, WSAGetLastError(), 0, message, 256, 0);

    printf("Connection status: %s", message);

    while(1) {
        memset(message, 0,sizeof(message));
        err = recv(sockfd, message, sizeof(message)-1,MSG_WAITALL);
        perror("Recieve status: ");
        pnum = atoi(message);
        printf("Number recieved: %d\n", pnum);
        if(pnum == 0)
            break;
        else {
            // begins checking if number is prime
            for(int i = 2; i < pnum; i++) { 
                if (pnum % i == 0) {
                    prime=0;
                    break;
                }
                else if (i == pnum - 1)
                    prime=1;
                   
                }
            sprintf(message, "%d", prime);
            n = send(sockfd, message, sizeof(message),MSG_OOB);
            perror("Sending status: ");
            }
        }
    
    
    closesocket(sockfd);
    return 0;
}