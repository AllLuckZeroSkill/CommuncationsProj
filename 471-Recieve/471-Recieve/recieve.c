#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PACKETSIZE 1024

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "init.h"

void create_and_bind_server_socket();
void server_receive_data(SOCKET s);
//Issa sucks balls
int main() {
	init_winsock();
	create_and_bind_server_socket();

	SOCKET s;
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);

	// Create Server Socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket: %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	else printf("\nUDP SERVER SOCKET CREATED");

	server_receive_data(s);

	printf("\n\n");
	return 0;
}

// Additional functions

void create_and_bind_server_socket()
{
	SOCKET s;
	struct sockaddr_in server;
	int server_len = sizeof(server);

	// Create and Bind Server Socket – server must be running first
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket: %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	else printf("\nUDP SERVER SOCKET CREATED");

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(80);

	// Bind Socket
	if (bind(s, (struct sockaddr*)&server, server_len) == SOCKET_ERROR) {
		printf("Bind failed with error code: %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("\nSERVER SOCKET BIND SUCCESS");

	// Wait for data
	printf("\nWaiting for data...");
}

void server_receive_data(SOCKET s)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	int recv_len;
	char packet[PACKETSIZE + 1];
	Sleep(5000);
	while (1) {
		fflush(stdout);
		memset(packet, '\0', PACKETSIZE);  // clear buffer of previously received data

		// RECEIVE DATA PACKET - blocking
		if ((recv_len = recvfrom(s, packet, PACKETSIZE + 1, 0, (struct sockaddr*)&si_other, &slen)) == SOCKET_ERROR) {
			printf("\nrecvfrom() failed with error code: %d\n", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		else {
			printf("\nSERVER Received packet IPaddr %s Port %d, seq no = %d: \n", inet_ntoa(si_other.sin_addr),
				ntohs(si_other.sin_port), packet[PACKETSIZE]);
		}
	}
}