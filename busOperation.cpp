
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
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

    fd = open("/dev/uio0", O_RDWR);
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
        mapped[i + 19] = 0xffffffff;
        mapped[i     ] = 0xffffffff;
    }

    munmap(map_addr, size_board);

    close(fd);
    return 0;
}

int busText(const char* a) {
    int fd;
    void *map_addr;
    int size_board;
    volatile unsigned int *mapped;

    fd = open("/dev/uio0", O_RDWR);
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
    mapped += 64;
    for (int i = 0; i < 512; i++) {
        mapped[i] = i % 128;
    }   
    mapped[0] = a[0];
    mapped[1] = a[1];
    mapped[2] = a[2];
    mapped[3] = a[3];

    munmap(map_addr, size_board);

    close(fd);
    return 0;
}
