#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "Communication_Bridge.h"
#include "busOperation.h"


int Send_2_Client(int fds, unsigned char boardmap[BOARDSIZE][BOARDSIZE], const char game_msg[], bool dummyline, const char text_msg[])
{
    int Sent_Bytenum;
    unsigned int i, j;
    //Server_Message *msg;
    char msg[BOARDSIZE * BOARDSIZE + 10 + 20 + 1];

    memcpy(msg, game_msg, strlen(game_msg));
    for (i = strlen(game_msg); i < 10; i++)
        msg[i] = ' ';
    memcpy(&msg[10], boardmap, BOARDSIZE * BOARDSIZE);
    i = 10 + BOARDSIZE * BOARDSIZE;
    memcpy(&msg[i], text_msg, strlen(text_msg));
    j = strlen(text_msg);
    i += j;
    for ( ; j < 20; j++, i++)
        msg[i] = ' ';
    msg[i] = '\0';
    /*for ( ; i < BOARDSIZE; i++)
      for (j = 0; j < BOARDSIZE; j++)
      msg[i * BOARDSIZE + j] = boardmap[i][j];

      for ( ; i < 10; i++)
      msg[i] = ' ';
      for ( ; i < strlen(text_msg); i++)
      msg[i] = text_msg[i];

      msg[i] = '\0';*/
    /*
       msg = (Server_Message *)malloc(sizeof(Server_Message));
       if ( boardmap )
       memcpy(msg->Map, boardmap, BOARDSIZE * BOARDSIZE);
       if ( game_msg ){
    //msg->game_msg = (unsigned char *)malloc(strlen(game_msg) * sizeof(unsigned char));
    strcpy(msg->game_msg, game_msg, strlen(game_msg));
    }
    printf("Send_2_Client game_message: %s\n", msg->game_msg);

    msg->dummyline = dummyline;
    if ( text_msg ){
    //msg->game_msg = (unsigned char *)malloc(strlen(text_msg) * sizeof(unsigned char));
    strcpy(msg->text_msg, text_msg, strlen(text_msg));
    }
    */
    if ( (Sent_Bytenum = send(fds, msg, sizeof(msg), 0)) < 0 ){
        perror("Server: SendError"); exit(1);
    }

    //if ( msg->game_msg ) free(msg->game_msg);
    //if ( msg->text_msg ) free(msg->text_msg);
    //free(msg);
    return Sent_Bytenum;

}

int Send_2_Server(int fds, char Operation[])
{
    int Sent_Bytenum;
    //unsigned int i;
    //char msg[11];
    /*Client_Message *msg;
      printf("Send_2_Server: %s\n", Operation);
      msg = (Client_Message *)malloc(sizeof(Client_Message));
      if ( Operation ){
    //msg->operation = (unsigned char *)malloc(strlen(Operation) * sizeof(unsigned char));
    memcpy(msg->operation, Operation, strlen(Operation));
    }
    */
    /*
       for (i = 0; i < strlen(Operation); i++)
       msg[i] = Operation[i];
       for ( ; i < 10; i++)
       msg[i] = ' ';
       msg[i] = '\0';
       printf("Send_2_Server: %s\n", msg);
       */
    Sent_Bytenum = send(fds, Operation, sizeof(Operation), 0);
    //if ( msg->operation ) free(msg->operation);
    //free(msg);
    return Sent_Bytenum;

}

int Reqs_4_Client(int fds, char buf[])
{
    int Recv_Bytenum;//, i;
    //unsigned char buf[255];//*buf;
    //buf = (unsigned char *)malloc(255);
    if ( (Recv_Bytenum = recv(fds, buf, sizeof(buf), 0)) > 0 ){
        buf[Recv_Bytenum] = '\0';
    }
    else{
        perror("Server: RecvError"); exit(1);
    }
    /*
       for (i = 0; i < Recv_Bytenum; i++)
       if( buf[i] == ' '){
       buf[i] = '\0';
       break;
       }
       */
    printf("Reqs_4_Client: %s.for %dBytes\n", buf, Recv_Bytenum);
    return Recv_Bytenum;
}


int Reqs_4_Server(int fds, char game_msg[])
{
    int Recv_Bytenum;
    unsigned int i, j;
    unsigned char Map[BOARDSIZE][BOARDSIZE];
    unsigned int Map_ram[38];
    char Test_msg[20];
    memset(Map, 0, BOARDSIZE * BOARDSIZE);
    memset(Map_ram, 0, 38);
    //Server_Message *sev_msg;
    //sev_msg = (Server_Message *)malloc(sizeof(Server_Message));
    unsigned char buf[450];//*buf;
    //buf = (unsigned char *)malloc(255);
    if ( (Recv_Bytenum = recv(fds, buf, sizeof(buf), 0)) > 0 ){
        printf("Reqs_4_Server_Recv_Bytenum: %d\n", Recv_Bytenum);
        //memcpy(sev_msg, buf, sizeof(Server_Message));
        //strcpy(game_msg, sev_msg->game_msg);
        for (i = 0; buf[i] != ' '; i++)
            game_msg[i] = buf[i];
        game_msg[i] = '\0';
        Recv_Bytenum = i;
        printf("Reqs_4_Server: %s.for %dBytes.\n", game_msg, Recv_Bytenum);
        memcpy(Map, &buf[10], BOARDSIZE * BOARDSIZE);
        printf("After Sending from server to client.\n");
        busMap(Map, Map_ram);
        busSend(Map_ram);
        j = 0;
        for (i = 371; j < 16; i++)
            Test_msg[j++] = buf[i];
        Test_msg[j] = '\0';
        if ( Test_msg[0] != ' ' )
            busText(Test_msg);

    }

    return Recv_Bytenum;

}
