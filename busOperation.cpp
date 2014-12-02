
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "busOperation.h"

#define nXILLY_BUS

void busMap(unsigned char PiecesMap[BOARDSIZE][BOARDSIZE], unsigned int busRam[38]) {
    unsigned int temp;
    unsigned int u32;
    int i, j;
    for (j = 0; j < 19; j++) {
        for (i = 0, temp = 0x00; i < 16; i++) {
            u32 = PiecesMap[j][i];
            u32 = u32 << 30;
            temp = temp >> 2 | u32;
        }
        busRam[j] = temp;
        for (i = 16, temp = 0x00; i < 19; i++) {
            u32 = PiecesMap[j][i];
            u32 = u32 << 4;
            temp = temp >> 2 | u32;
        }
        busRam[j + 19] = temp;
    }
}



int busSend(unsigned int busRam[38]) {
    int fd;
    void *map_addr;
    int size_board;
    volatile unsigned int *mapped;

    #ifdef XILLY_BUS
    fd = open("/dev/uio0", O_RDWR);
    #else
    fd = open("/dev/zero", O_RDWR);
    #endif
    if (fd < 0) {
        perror("Failed to open devfile");
        return 1;
    }
    size_board = sizeof(int) * 64 + sizeof(char) * 512;
    map_addr = mmap( NULL, size_board, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (map_addr == MAP_FAILED) {
        perror("Failed to mmap");
        return 1;
    }

    mapped = (volatile unsigned int *)map_addr;

    for (int i = 0; i < 19; i++) {    
        mapped[i + 19] = busRam[37 - i];
        mapped[i     ] = busRam[18 - i];
    }

    munmap(map_addr, size_board);

    close(fd);
    return 0;
}

int busText(const char* display_buffer, bool dummyline) {
    int fd;
    unsigned int i;
    void *map_addr;
    int size_board;
    volatile unsigned int *mapped;
    static int Current_Line = 0;

    #ifdef XILLY_BUS
    fd = open("/dev/uio0", O_RDWR);
    #else
    fd = open("/dev/zero", O_RDWR);
    #endif
    if (fd < 0) {
        perror("Failed to open devfile");
        return 1;
    }
    size_board = sizeof(int) * 64 + sizeof(char) * 512;
    map_addr = mmap( NULL, size_board, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (map_addr == MAP_FAILED) {
        perror("Failed to mmap");
        return 1;
    }

    mapped = (volatile unsigned int *)map_addr;
    mapped += MAPTEXT_OFFSET;

    if ( Current_Line >= 32 ){
        Current_Line = 0;
        for (i = 0; i < 512; i++)
            mapped[i] = 0;
    }

    mapped += Current_Line * 16;                                                               // Set the starting addr

    for (i = 0; i < strlen(display_buffer); i++)
        mapped[i] = display_buffer[i];
    for (;i < 16; i++)	
        mapped[i] = ' ';
    if ( dummyline )
        Current_Line++;


    munmap(map_addr, size_board);

    close(fd);
    return 0;
}
