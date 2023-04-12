#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PACKETSIZE 1028

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <time.h>
#include "init.h"
#include <Windows.h>

unsigned char* read_image_file(char* filename, unsigned long* fileLen);
SOCKET create_client_socket_and_send_data(unsigned char* buffer, int buffer_len);

int main() {
    init_winsock();

    char* filename = "image/test.jpg";
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

int send_packet(SOCKET s, const char* buffer, int buffer_len, struct sockaddr_in* si_other, int slen) {
    int sent = 0;
    int sequence_number = 0;
    LARGE_INTEGER frequency, start, end;
    long long int elapsed_time;

    QueryPerformanceFrequency(&frequency); // Get the performance counter frequency

    while (sent < buffer_len) {
        int bytes_to_send = min(buffer_len - sent, PACKETSIZE - sizeof(int));
        char packet[PACKETSIZE];
        memcpy(packet, &sequence_number, sizeof(int));  // Add sequence number to the packet
        memcpy(packet + sizeof(int), buffer + sent, bytes_to_send);

        QueryPerformanceCounter(&start); // Start the timer

        if (sendto(s, packet, bytes_to_send + sizeof(int), 0, (struct sockaddr*)si_other, slen) == SOCKET_ERROR) {
            printf("sendto() failed with error code: %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        else printf("\nsent packet with sequence number: %d", sequence_number);

        QueryPerformanceCounter(&end); // Stop the timer

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(s, &readfds);

        struct timeval tv;
        tv.tv_sec = 0.5;  // Timeout 2 seconds
        tv.tv_usec = 0;

        if (select(s + 1, &readfds, NULL, NULL, &tv) > 0) {
            char ack[PACKETSIZE];
            if (recvfrom(s, ack, PACKETSIZE, 0, (struct sockaddr*)si_other, &slen) == SOCKET_ERROR) {
                printf("\nrecvfrom() failed with error code: %d\n", WSAGetLastError());
                exit(EXIT_FAILURE);
            }

            int ack_seq;
            memcpy(&ack_seq, ack, sizeof(int));

            if (ack_seq == sequence_number) {
                sent += bytes_to_send;
                sequence_number++;  // Increment the sequence number
                printf("\nACK received for sequence number: %d", ack_seq);
            }
            else {
                printf("\nIncorrect ACK received. Resending packet with sequence number: %d", sequence_number);
            }
        }
        else {
            printf("\nTimeout, resending packet with sequence number: %d", sequence_number);
        }
        // Calculate the elapsed time in microseconds
        elapsed_time = (end.QuadPart - start.QuadPart) * 1000000LL / frequency.QuadPart;
        printf("\nElapsed time: %lld microseconds\n\n", elapsed_time);
    }
    return sent;
}




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
    si_other.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change this to receiver/server ip
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(80); // converts short to network byte

    // Send data in packets and handle ACKs and timeouts
    send_packet(s, buffer, buffer_len, &si_other, slen);

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