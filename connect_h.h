#ifndef CONNECT_H
#define CONNECT_H

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include "jsondecode.cpp"
#define DEFAULT_PORT 10000
#define DEFAULT_BUFLEN 2500

char recvbuf[DEFAULT_BUFLEN], sendbuf[DEFAULT_BUFLEN] = "Hola desde el planeta C";
int iResult, iSendResult;
int recvbuflen = DEFAULT_BUFLEN;
WSADATA wsaData;
sockaddr_in local;
SOCKET server, client;

void myConnect();

#endif // CONNECT_H
