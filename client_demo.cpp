/*
 ** client.cpp -- a stream socket client demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include "Communication_Bridge.h"
#include "busOperation.h"

#define PORT "3490" // the port client will be connecting to

#define MAXDATASIZE 100 // max number of bytes we can get at once

#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL | ICANON)

// get sockaddr, IPv4 or IPv6:


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void toUpper_s(char * str, int len)
{
    for(int i = 0; i < len; i++)
        str[i] = toupper(str[i]);
    //return str;
}

int main(int argc, char *argv[])
{
    int sockfd, j;
    char server_buf[MAXDATASIZE];
    char cmd[] = "GEN";
    struct addrinfo hints, *servinfo, *p;
    bool Game_over = false;
    int Recv_Bytenum, Send_Bytenum;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 3) {
        fprintf(stderr,"usage: client hostname msg\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                        p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    /*
     *  Define the argv[2] as User name
     *  the first messgae is not gonna be sent
     */
    for (j = 0; j < 32; j++)
        busText("                ");
    send(sockfd, argv[2], strlen(argv[2]) + 1, 0);
    Game_over = false;
    while( !Game_over ){
        /* Wait for server to wake me up */
        //if ( (Recv_Bytenum = recv(sockfd, server_buf, sizeof(server_buf), 0)) > 0 ){
        if ( (Recv_Bytenum = Reqs_4_Server(sockfd, server_buf)) > 0 ){
            //server_buf[Recv_Bytenum] = '\0';
            printf("%s\n", server_buf);
            if ( strcmp(server_buf, "GAMEOVER") == 0 ){
                Game_over = true;
                continue;
            }
            else if ( strcmp(server_buf, "WAKE") != 0) {
                continue;
            }
        }
        else{
            perror("Client: Wait for wake error!");
            exit(1);
        }

        /* Read the client operation */
        if ( strcmp(cmd, "GEN") == 0 ){
            //if ( (Send_Bytenum = send(sockfd, cmd, strlen(cmd), 0)) < 0 ){
            if ( (Send_Bytenum = Send_2_Server(sockfd, cmd)) < 0 ){
                perror("Client: Send Error");
                exit(1);
            }
            else{
                printf("send gen\n");
                //break;
            }
        }
        }

        close(sockfd);

        return 0;
    }
