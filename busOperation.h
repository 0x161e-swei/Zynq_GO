

#define BOARDSIZE 19
extern unsigned int busRam[38];

extern void busMap(unsigned char PiecesMap[BOARDSIZE][BOARDSIZE], unsigned int busRam[38]);
extern int busSend(unsigned int busRam[38]);
extern int busText(const char* a);