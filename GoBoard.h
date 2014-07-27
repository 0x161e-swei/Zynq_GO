/*
 * GoBoard.h
 *
 *  Created on: Apr 10, 2014
 *      Author: gongbingchen
 */

#ifndef GOBOARD_H_
#define GOBOARD_H_

#define BOARDSIZE 19

struct PiecesPosition
{
    unsigned char status;
    unsigned char piecesx;
    unsigned char piecesy;
};

class GoBoard
{
private:
    int fd;
    bool bPreviousReady;
public:
    unsigned char PiecesMap[BOARDSIZE][BOARDSIZE];
    GoBoard();
    virtual ~GoBoard(){
        close(fd);
    }
    PiecesPosition waitInput();
    void clear();
    void refresh();
    void setPieces(PiecesPosition position, unsigned char value);
};

#endif /* GOBOARD_H_ */
