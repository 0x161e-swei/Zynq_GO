
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>

#include "GoBoard.h"
#include "GTPClient.h"

#include <stdlib.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define GBD_IOC_MAGIC 'w' //定义类型
#define GBD_IOCSETBD _IOW(GBD_IOC_MAGIC, 0, unsigned char)
#define GBD_IOCGQSET _IOR(MEM_IOC_MAGIC, 1, int)

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

using namespace std;


/* Global Variables */

GoBoard goboard;
GTPClient client("gnugo.gtp");
int fds1, fds2;
int bc, wc;
char *User_name1, *User_name2;


void sigchld_handler(int s)
{
    s = 0;
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

GoBoard::GoBoard(): bPreviousReady(true)
{
    //fd = open("/dev/goboard", O_RDWR);
    //if(fd<0){
    //  cerr << "open: " << strerror(errno) << std::endl;
    //  abort();
    //}
    //clear();
}

//PiecesPosition GoBoard::waitInput()
//{
//  PiecesPosition position = {0,};
//
//  do{
//      read(fd, &position, sizeof(position));
//      bool bReady = position.status & (1<<7);
//      //cout<<"bPreviousReady\t"<<bPreviousReady<<"\tbReady\t"<<bReady<<endl;
//      if(!bPreviousReady && bReady){
//          bPreviousReady = true;
//          position.piecesy--;
//          if(position.piecesx < BOARDSIZE && position.piecesy < BOARDSIZE)
//              break;
//      }else bPreviousReady = bReady;
//
//  }while(!usleep(500));
//  return position;
//}
//
//void GoBoard::setPieces(PiecesPosition position, unsigned char value)
//{
//  cout<<"setPieces "<<(int)position.piecesx<<","<<(int)position.piecesy<<"="<<(int)value<<endl;
//
//  PiecesMap[position.piecesy][position.piecesx] = value;
//
//  cout<<"set write: "<<write(fd, &PiecesMap[position.piecesy], sizeof(unsigned char)*BOARDSIZE)<<endl;
//
//  cout<<"set ioctl: "<<ioctl(fd, GBD_IOCSETBD, BOARDSIZE-position.piecesy-1)<<endl;
//}
//
//void GoBoard::clear()
//{
//  memset(PiecesMap, 0, sizeof(PiecesMap));
//  refresh();
//}
//
//void GoBoard::refresh()
//{
//  for (int i = 0; i < 19; ++i)
//  {
//      write(fd, &PiecesMap[i], sizeof(unsigned char)*BOARDSIZE);
//      
//      ioctl(fd, GBD_IOCSETBD, BOARDSIZE-i-1);
//  }
//}

int waitClient (int sockfd) {
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int new_fd;

    sin_size = sizeof their_addr; 
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

    if (new_fd  == -1) {
        perror("accept");
        return -1;
    }
    printf("Client conneted\n");

    char s[INET6_ADDRSTRLEN];
    inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
    printf("server: got connection from %s\n", s);

    return new_fd;
}

int initSocket(const char* addr) {

    int sockfd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sigaction sa;
    int yes=1;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(addr, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                        p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                    sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    return sockfd;
}

void Set_Move_Position(char *buf, PiecesPosition &position, int color)
{
    int j;
    if ( buf[0] >= 'A' && buf[0] <= 'H' )                                                           // Calculate the addr of x
        position.piecesx = buf[0] - 'A';    
    else if ( buf[0] >= 'J' && buf[0] <= 'T' )                                                      // No I addr on board
        position.piecesx = buf[0] - 'A' - 1;                                                        
    position.piecesy = 0;
    j = 1;
    while( buf[j] != '\0' ){
        position.piecesy = position.piecesy * 10 + buf[j] - '0';
        j++;
    }
    position.piecesy -= 1;                                                                          // Convert 1~19 to 0~18
    position.status = color;                                                                        // St color
}

int Client_Comminication(int fds, int color, PiecesPosition &position)
{
    int Sent_Bytenum, Recv_Bytenum;
    bool RoundDone, Move_ret , Quit;                                                                // RoundDone to mark if the user finished current round
    char buf[255];
    if ( (Sent_Bytenum = send(fds, "WAKE", 4, 0)) < 0 ){    
        perror("Server: SendError"); exit(1);
    }
    RoundDone = false;
    while ( !RoundDone && (Recv_Bytenum = recv(fds, buf, sizeof(buf), 0)) > 0 ){
        buf[Recv_Bytenum] = '\0';
        if ( color == 1)                                                                            // MAIMENG
            printf("1, buf: %s\n", buf);
        else 
            printf("2, buf :%s\n", buf);
        if ( strcmp(buf, "QUIT") == 0 ){
            send(fds1, "GAMEOVER", 9, 0);                                                           // Send gameover message to both client
            send(fds2, "GAMEOVER", 9, 0);
            if ( color == 1 )
                printf("USER: %s QUIT!\n", User_name1);
            else 
                printf("USER: %s QUIT!\n", User_name2);
            Quit = true;                                                                            // Quit = TRUE when user send quit message
            RoundDone = true;
        }
        else if ( strcmp(buf, "GEN") == 0 ) {
            cout << client.GenMove(color, position) << endl;
            cout << client.ShowBoard(goboard.PiecesMap, bc, wc) << endl;        
            RoundDone = true;
        }
        else if ( strcmp(buf, "PASS") == 0 ) {
            if ( color == 1 )
                printf("USER: %s PASS!\n", User_name1);
            else 
                printf("USER: %s PASS!\n", User_name2);
            RoundDone = true;
        }
        else if ( strlen(buf) == 2 || strlen(buf) == 3 ){                                           // Set piece
            //printf("Piece set at %s\n", buf);
            Set_Move_Position(buf, position, color);                                                // Set position to an address user want
            Move_ret = client.Move(position, color);                                                // Attempt to set piece
            if ( !Move_ret ){                                                                       
                printf("Illegal location, piece already exist!\n");
                if ( (Sent_Bytenum = send(fds, "FAIL", 4, 0)) < 0 ){                                // Inform the client with a invalid position
                    perror("Server: SendError"); exit(1);
                }
            }
            else{
                printf("Usr1 set piece successfully at %s\n", buf);
                if ( (Sent_Bytenum = send(fds, "SUCCEED", 8, 0)) < 0 ){                             // Inform the client with success information
                    perror("Server: SendError"); exit(1);
                }
                cout << client.ShowBoard(goboard.PiecesMap, bc, wc) << endl; 
                RoundDone = true;    
            }                    
        }
    }
    return Quit;  
} 

void Set_username(int fds){
    char *buf;
    int Len;
    buf = (char *)malloc(255 * sizeof(char));
    if ( (Len = recv(fds, buf, 255, 0)) < 0 ){
        perror("Username receive error"); exit(1);
    }  
    else{
        buf[Len] = '\0';
        if ( fds == fds1 ){
            User_name1 = (char *)malloc(strlen(buf) + 1);
            strcpy(User_name1, buf);
        }
        else{
            User_name2 = (char *)malloc(strlen(buf) + 1);
            strcpy(User_name2, buf);
        } 
    }
    free(buf);
}

int main()
{

    int piecesTurn = 1;
    bool ret;
    cout<<"Initialized GoBoard"<<endl;

    //cout<<client.Ping()<<endl;

    int sockfd;
    sockfd = initSocket("0.0.0.0");

    printf("server: waiting for connections...\n");
    
    fds1 = waitClient(sockfd); //block here
    fds2 = waitClient(sockfd); //block here

    /*  Here to receive the first two message from fds1 and fds2
     *
     *  Usr1 & Usr2
     */
    Set_username(fds1);
    Set_username(fds2);

    int i = 300;                                                                                        
    bool Quit;
        
    PiecesPosition position = { 0, 1, 1 };

    while (i--) {
        Quit = Client_Comminication(fds1, 1, position);
        if ( Quit == true ) 
            break;
        usleep(5000);
        Quit = Client_Comminication(fds2, 2, position);
        if ( Quit == true )
            break;
    }
    printf("Game_Over\n");
    if ( User_name1 ) free(User_name1);
    if ( User_name2 ) free(User_name2);
    return 0;               // ???

/*    if (!fork()) { // this is the child process
        close(sockfd); // child doesn't need the listener
        if (send(new_fd, "Hello, world!", 13, 0) == -1)
            perror("send");
        close(new_fd);
        exit(0);
    }*/
    //close(new_fd);  // parent doesn't need this

    for(;;){
        //PiecesPosition position = goboard.waitInput();
        PiecesPosition position = { 0, 1, 1 };
        //client.Move(position, piecesTurn);
        //cout<<ret<<endl;

        ret = 1;
        if(ret){
            //goboard.setPieces(position, piecesTurn);
            cout << client.GenMove(1, position) << endl;
            cout<<client.ShowBoard(goboard.PiecesMap, bc, wc)<<endl;
            //goboard.refresh();

            //if(++piecesTurn > 2) piecesTurn = 1;
            cout<<client.GenMove(2, position)<<endl;

            cout<<client.ShowBoard(goboard.PiecesMap, bc, wc)<<endl;

            //goboard.refresh();

            if(++piecesTurn > 2) piecesTurn = 1;
        }
    }

    return 0;
}
