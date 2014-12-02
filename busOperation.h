#ifndef BUSOPERATION_H_
#define BUSOPERATION_H_

#define BOARDSIZE 19
#define MAPTEXT_OFFSET 64

extern unsigned int busRam[38];

extern void busMap(unsigned char PiecesMap[BOARDSIZE][BOARDSIZE], unsigned int busRam[38]);
extern int busSend(unsigned int busRam[38]);
extern int busText(const char* a, bool dummyline = true);

#endif /* BUSOPERATION_H_  */
