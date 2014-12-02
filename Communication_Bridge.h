#ifndef COMMUNICATION_BRIDGE_H_
#define COMMUNICATION_BRIDGE_H_

#define BOARDSIZE 19

struct Server_Message{
    char game_msg[10];
    unsigned char Map[BOARDSIZE][BOARDSIZE];
    bool dummyline;
    char text_msg[20]; 
};

struct Client_Message{
    char operation[10];
    
};


extern int Send_2_Client(int fds, unsigned char boardmap[BOARDSIZE][BOARDSIZE], const char game_msg[], bool dummyline, const char text_msg[]);
extern int Send_2_Server(int fds, char Operation[]);
extern int Reqs_4_Client(int fds, char buf[]);
extern int Reqs_4_Server(int fds, char game_msg[]);

#endif /* COMMUNICATION_BRIDGE_H_ */