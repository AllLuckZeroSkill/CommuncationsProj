#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PACKETSIZE 1024

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include "init.h"

unsigned char* read_image_file(char* filename, unsigned long* fileLen);
SOCKET create_client_socket_and_send_data(unsigned char* buffer, int buffer_len);

int main() {
    init_winsock();

    char* filename = "image/umlogo.jpg";
    unsigned long fileLen;
    unsigned char* buffer = read_image_file(filename, &fileLen);
    if (buffer == NULL) {
        printf("\nFailed to read image file");
        exit(1);
    }

    SOCKET s = create_client_socket_and_send_data(buffer, fileLen);

    free(buffer);  // free the memory allocated for the buffer 

    printf("\nFile Name: %s \nFile length: %lu\n", filename, fileLen);

    printf("\n\n");
    return 0;
}

// Additional functions

SOCKET create_client_socket_and_send_data(unsigned char* buffer, int buffer_len) {
    SOCKET s;
    struct sockaddr_in si_other;
    int slen = sizeof(si_other);
    u_long noBlock;

    // Create Client (Primary) Socket And Send Data
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("UDP CLIENT SOCKEET CREATED.\n");

    // Initialize socket struct - Non-blocking Client
    noBlock = 1;
    ioctlsocket(s, FIONBIO, &noBlock);
    si_other.sin_addr.s_addr = inet_addr("127.0.0.1");
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(80);

    if (sendto(s, buffer, buffer_len, 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR) {
        printf("sendto() failed with error code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    else printf("\nsent buffer");

    return s;
}

unsigned char* read_image_file(char* filename, unsigned long* fileLen) {
    FILE* fp;
    char* buffer;

    //  open image file 
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("\nError opening image file");
        return NULL;
    }

    // get file size 
    fseek(fp, 0, SEEK_END);
    *fileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // allocate memory for buffer 
    buffer = (char*)malloc(*fileLen + 1);
    if (!buffer) {
        printf("\nError allocating memory for buffer");
        fclose(fp);
        return NULL;
    }

    //  read file data into buffer 
    fread(buffer, *fileLen, 1, fp);
    fclose(fp);

    return buffer;
}
