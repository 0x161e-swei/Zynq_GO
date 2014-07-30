/*
 * GTPClient.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: gongbingchen
 *  Modified on:Jun 30, 2014
 *      Author: taoj
 *      Author: weishijia
 */
#include "GTPClient.h"
#include <iostream>

using namespace std;

const char GTPClient::PositionXTable[] = "ABCDEFGHJKLMNOPQRST";
const char * GTPClient::ColorString[] = {NULL, "black", "white"};

void GTPClient::GTPFork()
{
    int pipefd0[2],pipefd1[2];

    pipe(pipefd0);
    pipe(pipefd1);

    pid = fork();
    if(pid){
        //parent
        close(pipefd0[0]);
        close(pipefd1[1]);
        clientIn = fdopen(pipefd0[1], "w");
        clientOut = fdopen(pipefd1[0], "r");
    }else{
        //child
        //close(pipefd0[1]);
        //close(pipefd1[0]);

        if(dup2(pipefd0[0], STDIN_FILENO) != STDIN_FILENO){
            abort();
        }
        //close(pipefd0[0]);

        if(dup2(pipefd1[1], STDOUT_FILENO) != STDOUT_FILENO){
            abort();
        }
        //close(pipefd1[1]);

        execl(prog.c_str(), prog.c_str(), NULL);
        //execlp("gnugo", "gnugo", "--mode", "gtp", "--level", "0", NULL);
        fprintf(stderr, "an error occurred in execl\n");
        abort();
    }
}

bool GTPClient::GTPCommand(string cmd, stringstream &rets)
{
    bool ret = false;
    int rseq;
    char cStatus;

    cout<<"cmd: "<<cmd<<endl;
    fprintf(clientIn, "%d %s\n", seq, cmd.c_str());
    fflush(clientIn);

    char buf[256];
    //cin >> cStatus;
    //cout << cStatus << endl;
    fscanf(clientOut, "%c%d%*c", &cStatus, &rseq);
    fgets(buf, sizeof(buf), clientOut);

    if(rseq == seq){
        rets<<buf;
        seq++;
        do{
            fgets(buf, sizeof(buf), clientOut);
            rets<<buf;
        }while(*buf != '\n');
        ret = cStatus == '=';
    }

    return ret;
}

bool GTPClient::Ping()
{
    const string pingLoad("ping");
    const string pingCmd("echo");

    stringstream rets;
    bool ret = false;
    string result;

    ret = GTPCommand(pingCmd + " " + pingLoad, rets);

    if( ret && fgetc(clientOut) == '\n'){
        rets>>result;
        cout<<" Return: "<<result<<endl;
        ret = (pingLoad == result);
    }

    fflush(clientOut);
    return ret;
}

bool GTPClient::Move(PiecesPosition position, int color)
{
    const string pingCmd("play");
    const string pingColor(ColorString[color]);

    stringstream rets;
    bool ret = false;
    stringstream pingPosition;
    pingPosition<<pingCmd<<" "
        <<pingColor<<" "
        <<PositionXTable[position.piecesx]
        <<position.piecesy+1;

    ret = GTPCommand(pingPosition.str(), rets);

    fflush(clientOut);

    return ret;
}

bool GTPClient::GenMove(int color, PiecesPosition &position)
{
    const string pingCmd("genmove");
    const string pingColor(ColorString[color]);

    char px;
    int py;
    stringstream rets;
    bool ret = false;

    string pingPosition = pingCmd + " ";
    pingPosition += pingColor;

    ret = GTPCommand(pingPosition, rets);
    position.status = ret;

    if(ret){
        cout<<"GenMove: "<<rets.str()<<endl;

        rets>>px>>py;

        for(size_t i=0; i<sizeof(PositionXTable); i++)
            if(PositionXTable[i] == px){
                position.piecesx = i;
                break;
            }
        position.piecesy = py-1;
    }

    fflush(clientOut);

    return ret;
}

bool GTPClient::ShowBoard(unsigned char PiecesMap[BOARDSIZE][BOARDSIZE], int &bc, int &wc)
{
    const string pingCmd("showboard");
    stringstream rets;
    string result;
    bool ret = false;
    int pos;

    ret = GTPCommand(pingCmd, rets);

    if(ret){
        rets.seekg(42);

        while(rets>>pos && pos > 0){
            pos--;
            for(int i=0; i<BOARDSIZE; i++){
                rets>>result; cout<<result;
                if(result == "X") PiecesMap[pos][i] = 1;
                else if(result == "O") PiecesMap[pos][i] = 2;
                else PiecesMap[pos][i] = 0;
            }
            cout<<endl;
            rets>>pos;
            if(pos == 11){
                rets>>result
                    >>result
                    >>result
                    >>result;
                rets>>bc;
                rets>>result;
            }if(pos == 10){
                rets>>result
                    >>result
                    >>result
                    >>result;
                rets>>wc;
                rets>>result;
            }
        }

    }

    return ret;

}

string GTPClient::ShowResult()
{
    const string pingCmd("final_score");
    stringstream rets;
    string result;
    bool ret;

    ret = GTPCommand(pingCmd, rets);

    if ( ret ){
        rets >> result;
    }

    return result;

}

GTPClient::GTPClient(const string prog): prog(prog), seq(0)
{
    GTPFork();
}

GTPClient::~GTPClient()
{
    fclose(clientIn);
    fclose(clientOut);
}
