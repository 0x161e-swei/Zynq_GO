/*
 * GTPClient.h
 *
 *  Created on: Apr 10, 2014
 *      Author: gongbingchen
 */

#ifndef GTPCLIENT_H_
#define GTPCLIENT_H_

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <sstream>

#include "GoBoard.h"

class GTPClient{
private:
    static const char PositionXTable[];
    static const char * ColorString[];

    pid_t pid;
    const std::string prog;
    FILE *clientIn;
    FILE *clientOut;
    int seq;
    void GTPFork();
    bool GTPCommand(std::string cmd, std::stringstream &rets);
public:
    GTPClient(const std::string prog);
    bool Ping();
    bool Move(PiecesPosition position, int color);
    bool GenMove(int color, PiecesPosition &position);
    bool ShowBoard(unsigned char PiecesMap[BOARDSIZE][BOARDSIZE], int &bc, int &wc);
    ~GTPClient();
};

#endif /* GTPCLIENT_H_ */
