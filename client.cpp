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

#include "Communication_Bridge.h"
#include "busOperation.h"

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void toUpper_s(char *str, int len)
{
    for(int i = 0; i < len; i++)
        str[i] = toupper(str[i]);
    //return str;
}

void Usage_info(char *cmd, int d)
{
    if ( d == 0 )
        printf("Usage: enter GEN for server hint, PASS for pass current round, or a position to located piece, eg. D13.\n");
    else if ( d == 1)
        printf("Set piece: A~H or J ~ T and num from 1 to 19, eg. D13.\n");
    else 
        printf("Illegal position, try another position please.\n");         
    scanf("%s", cmd);
    toUpper_s(cmd, strlen(cmd) + 1);  
}

int main(int argc, char *argv[])
{
    int sockfd, j;  
    char server_buf[MAXDATASIZE], *cmd;
    struct addrinfo hints, *servinfo, *p;
    bool Game_over = false;
    int Recv_Bytenum, Send_Bytenum, num, i;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 3) {
        fprintf(stderr,"usage: client hostname username\n");
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
     *  i.e. the first messgae is gonna be sent as username
     *  instead of operation message
     *
     */
    for (j = 0; j < 32; j++)
        busText("                "); 
    Send_2_Server(sockfd, argv[2]);                      // set username
    cmd = (char *)malloc(15 * sizeof(char));
    Game_over = false;
    while( !Game_over ){
        /* Wait for server to wake me up */
        printf("Waiting for wake\n");
        //if ( (Recv_Bytenum = recv(sockfd, server_buf, sizeof(server_buf), 0)) > 0 ){
        if ( (Recv_Bytenum = Reqs_4_Server(sockfd, server_buf))){
            //server_buf[Recv_Bytenum] = '\0';
            printf("%s\n", server_buf);
            if ( strcmp(server_buf, "GAMEOVER") == 0 ){
                Game_over = true;
                continue;
            }
            else if ( strcmp(server_buf, "WAKE") != 0)
                continue;       
        }
        else{
            perror("Client: Wait for wake error!"); exit(1);
        }

        /* Read the client operation */

        scanf("%s", cmd);
        toUpper_s(cmd, strlen(cmd) + 1);        
        while ( strcmp(cmd, "QUIT") != 0 && !Game_over ){
            if ( strcmp(cmd, "GEN") == 0 ){                                 // Radomly setpeice
                //if ( (Send_Bytenum = send(sockfd, cmd, strlen(cmd), 0)) < 0 ){
                if ( (Send_Bytenum = Send_2_Server(sockfd, cmd)) < 0 ){
                    perror("Client: Send Error"); exit(1);
                }
                else{
                    printf("gen sent\n");
                    break;
                }
            }
            else if ( strcmp(cmd, "PASS") == 0 ){
                //if ( (Send_Bytenum = send(sockfd, cmd, strlen(cmd), 0)) < 0 ){
                if ( (Send_Bytenum = Send_2_Server(sockfd, cmd)) < 0 ){
                    perror("Client: Send Error"); exit(1);
                }
                else break;
            }  
            else if ( strlen(cmd) == 2 || strlen(cmd) == 3 ){
                if ( ((cmd[0] >= 'A' && cmd[0] <= 'H') || (cmd[0] >= 'J' && cmd[0] <= 'T')) ){
                    num = 0;
                    i = 1;
                    while( cmd[i] != '\0'){
                        num = num * 10 + cmd[i] - '0';
                        i++;
                    }
                    if ( num >= 1 && num <= 19 ){                        
                        while ( i > 1 ){
                            cmd[--i] = '0' + num % 10;
                            num /= 10;
                        }
                        /*
                           if ( (Send_Bytenum = send(sockfd, cmd, strlen(cmd), 0)) < 0 ){
                           perror("Client: Send Error"); exit(1);
                           }
                           else{
                           if ( (Recv_Bytenum = recv(sockfd, server_buf, sizeof(server_buf), 0)) > 0 ){
                           server_buf[Recv_Bytenum] = '\0';
                           if ( strcmp(server_buf, "SUCCEED") == 0 ){
                           printf("Succeed received!\n");
                           break;
                           }
                           else Usage_info(cmd, 2);     
                           }
                           else perror("Client: Wait after move error!"); exit(1);
                           }
                           */
                        if ( Send_2_Server(sockfd, cmd) < 0 ){
                            perror("Client: Send Error"); exit(1);
                        }
                        else{
                            usleep(50000);
                            if ( (Recv_Bytenum = Reqs_4_Server(sockfd, server_buf)) > 0){
                                //server_buf[Recv_Bytenum] = '\0';
                                if ( strcmp(server_buf, "SUCCEED") == 0 ){
                                    printf("Succeed received!\n");
                                    break;
                                }
                                else Usage_info(cmd, 2);  
                                printf("After_move buffer: %s", server_buf); 
                            }
                            else{
                                perror("Client: Wait after move error!"); exit(1);       
                            }
                        }
                    }
                    else Usage_info(cmd, 1);
                }
                else Usage_info(cmd, 0);
            }
            else Usage_info(cmd, 0);
        }
        
        if ( strcmp(cmd, "QUIT") == 0 ){
            //if ( (Send_Bytenum = send(sockfd, cmd, strlen(cmd), 0)) < 0 ){
            //    perror("Client: Send Error"); exit(1);
            //}
            if ( Send_2_Server(sockfd, cmd) < 0){
                perror("Client: Send Error"); exit(1); 

                }
            }    
        }    

            if ( cmd ) free(cmd);
            close(sockfd);

            return 0;
    }
