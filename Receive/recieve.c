#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PACKETSIZE 1028

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "init.h"

SOCKET create_and_bind_server_socket();
void server_receive_data(SOCKET s);
//change
int main() {
    init_winsock(); // initialize winsock

    SOCKET s = create_and_bind_server_socket();
    server_receive_data(s);

    printf("\n\n");
    return 0;
}

// create socket and wait for data to come in if bind is successful
SOCKET create_and_bind_server_socket() {
    SOCKET s;
    struct sockaddr_in server;
    int server_len = sizeof(server);

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    else printf("\nUDP SERVER SOCKET CREATED");

    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change this to receiver/server ip
    server.sin_family = AF_INET;
    server.sin_port = htons(80);

    if (bind(s, (struct sockaddr*)&server, server_len) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    puts("\nSERVER SOCKET BIND SUCCESS");

    printf("\nWaiting for data...");

    return s; 
}

// receives data in packets and inputs each packet into an output file
void server_receive_data(SOCKET s)
{
    struct sockaddr_in si_other;
    int slen = sizeof(si_other);
    int recv_len;

    FILE* received_image = fopen("received_image.jpg", "wb");
    if (received_image == NULL) {
        printf("\nError opening received_image.jpg for writing");
        exit(EXIT_FAILURE);
    }

    int expected_seq_no = 0;

    while (1) {
        fflush(stdout);
        char packet[PACKETSIZE];

        // RECEIVE DATA PACKET - blocking
        if ((recv_len = recvfrom(s, packet, PACKETSIZE, 0, (struct sockaddr*)&si_other, &slen)) == SOCKET_ERROR) {
            printf("\nrecvfrom() failed with error code: %d\n", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        else {
            int seq_no;                   // sequence number extracted from end of packet and put into int variable
            memcpy(&seq_no, packet, sizeof(int));
            printf("\nSERVER Received packet IPaddr %s Port %d, seq no = %d: \n", inet_ntoa(si_other.sin_addr),
                ntohs(si_other.sin_port), seq_no);

            // writes received packet data into file if sequence number received = sequence number expected
            if (seq_no == expected_seq_no) {
                fwrite(packet + sizeof(int), 1, recv_len - sizeof(int), received_image);
                expected_seq_no++;

                // SEND ACK
                char ack[PACKETSIZE];
                memcpy(ack, &seq_no, sizeof(int));          // copy received sequence number into ack to send to client
                if (sendto(s, ack, sizeof(int), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR) {
                    printf("\nsendto() failed with error code: %d", WSAGetLastError());
                    exit(EXIT_FAILURE);
                }
                else printf("\nACK sent for sequence number: %d\n", seq_no);
            }
            else {
                printf("\nOut of order packet. Expected sequence number: %d, received: %d\n", expected_seq_no, seq_no);
            }
        }

        if (expected_seq_no > 9) break; // break out of loop
    }
    fclose(received_image);

    printf("\n\nImage Received ");
}