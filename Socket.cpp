#include "functions.h"

#define DEFAULT_BUFLEN 1024

int iFamily = AF_INET;
int iType = 1;
int iProtocol = IPPROTO_TCP;
int boundToPort = -1;
std::string clientResponse = "";

SOCKET sock = INVALID_SOCKET;
SOCKET clientSock = INVALID_SOCKET;

bool isConnectedToClient() { return (boundToPort!=-1)&&(clientSock!=INVALID_SOCKET) ; }

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)

#ifdef _WIN32
#pragma comment(lib,"Ws2_32.lib")

//This is a stripped down version of the code found at:
//https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket

WSADATA wsaData = {0};
int iResult;
char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;

sockaddr_in service;

int StartSocket(int port) {
	printFormatted("SERVER","Info","Starting up server");
	if (!OpenSocket())     { return 1; }
	if (!BindSocket(port)) { return 2; }
	if (!ListenOnSocket()) { return 3; }
	return 0;
}

bool OpenSocket() {
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printFormatted("SERVER","Error","WSAStartup failed with error: " + std::to_string(iResult));
        return false;
    }
	
	sock = socket(iFamily, iType, iProtocol);
    if (sock != INVALID_SOCKET) {
		printFormatted("SERVER","Success","Successfully opened socket");
		return true;
	}
	
	printFormatted("SERVER","Error","Failed to start socket with error: " + std::to_string(WSAGetLastError()));
	WSACleanup();
	return false;
}

bool CloseSocket() { bool result = CloseSocket(sock); sock == INVALID_SOCKET; return result; }
bool CloseClientSocket() { bool result = CloseSocket(clientSock); clientSock == INVALID_SOCKET; return result; }
bool CloseSocket(SOCKET i) {
	boundToPort = -1;
	iResult = closesocket(i);
	if (iResult != SOCKET_ERROR) {
		printFormatted("SERVER","Success","Closed socket");
		return true;
	}
	printFormatted("SERVER","Error","Failed to close socket with error: " + std::to_string(WSAGetLastError()));
	WSACleanup();
	return false;
}

bool BindSocket(int port) {
	if (sock == INVALID_SOCKET) { return false; }
	
	service.sin_family = iFamily;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(port);
	
	iResult = bind(sock, (SOCKADDR *) &service, sizeof (service));
	if (iResult == SOCKET_ERROR) {
		printFormatted("SERVER","Error","Failed to bind socket to port with error: " + std::to_string(WSAGetLastError()));
		CloseSocket();
		return false;
	}
	
	printFormatted("SERVER","Success","Bound socket to port: " + std::to_string(port));
	boundToPort = port;
	return true;
}

bool ListenOnSocket() {
	if (iProtocol == IPPROTO_UDP) { printFormatted("SERVER","Warning","UDP connection does not support listening"); return false; }
	if (sock == INVALID_SOCKET) { return false; }
	if (boundToPort == -1) { return false; }
	
	if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
		printFormatted("SERVER","Error","Could not listen on port: " + std::to_string(boundToPort));
		print(std::to_string(WSAGetLastError()));
		return false;
	}
	
	printFormatted("SERVER","Log","Listening on socket...");
	return true;
}

bool AcceptClient() {
	if (sock == INVALID_SOCKET) { printFormatted("SERVER","ERROR","Server socket not initialized"); return false; }
	if (boundToPort == -1) { printFormatted("SERVER","ERROR","Server socket not bound to port"); return false; }
	
	// Try to receive some data, this is a blocking call
	clientSock = accept(sock, NULL, NULL);
	if (clientSock == INVALID_SOCKET) {
        printFormatted("SERVER","Warning","Client accept failed with error: " + std::to_string(WSAGetLastError()));
        CloseSocket(sock);
        return false;
    } else {
        printFormatted("SERVER","Success","Client connected");
    }
	
    CloseSocket(sock);
    return true;
}

std::string GetClientResponse() {
	if (clientSock == -1) { printFormatted("SERVER","ERROR","Server socket not initialized"); return "INTERNAL ERROR"; }
	if (boundToPort == -1) { printFormatted("SERVER","ERROR","Server socket not bound to port"); return "INTERNAL ERROR"; }

	char recvBuff[DEFAULT_BUFLEN] = {0};
	std::string clientResponse;
	long timer = time(NULL);
	while(time(NULL) - timer < 5) {
		int bytesRead = recv(clientSock, recvBuff, DEFAULT_BUFLEN, 0);
		if (bytesRead > 0) {
			clientResponse += std::string(recvBuff, bytesRead);
		}
		if (clientResponse.find("\n") != std::string::npos) {
			std::string result = clientResponse.substr(0, clientResponse.find("\n"));
			clientResponse = clientResponse.substr(clientResponse.find("\n") + 1, clientResponse.length() - 1);

			return result;
		}
	}

	return "";
}

bool SendToClient(std::string dataToSend) {
	if (clientSock == INVALID_SOCKET) { printFormatted("SERVER","ERROR","Client socket not initialized"); return false; }
	
	dataToSend += "\n";
	
	char msg[dataToSend.length() + 1];
	strcpy(msg, dataToSend.c_str());
	
	iResult = send(clientSock, msg, dataToSend.length(), 0);
	if (iResult != SOCKET_ERROR) { return true; }
	
    printFormatted("SERVER","ERROR","Client send failed: " + std::to_string(WSAGetLastError()));
    CloseSocket(clientSock);
    return false;
}
#endif

#ifdef __linux__

int sock = -1;
int clientSock = -1;

int StartSocket(int port) {
	printFormatted("SERVER","Info","Starting up server");
	if (!OpenSocket())     { return -1; }
	if (!BindSocket(port)) { return -2; }
	if (!ListenOnSocket()) { return -3; }
	return 0;
}

bool OpenSocket() {
	sock = socket(iFamily, iType, iProtocol);
	if (sock < 0) {
		printFormatted("SERVER","Error","Failed to open socket");
	} else {
		printFormatted("SERVER","Success","Successfully opened socket");
	}
	return bool(sock >= 0);
}

bool BindSocket(int port) {
	if (sock == -1) { return false; }
	
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	
	int result = bind(sock, (struct sockaddr *) &sin, sizeof sin);
	if (result == 0) {
		boundToPort = port;
		printFormatted("SERVER","Success","Bound socket to port: " + std::to_string(port));
		return 1;
	}

	boundToPort = -1;
	printFormatted("SERVER","Error","Failed to bind to port: " + std::to_string(port));
	CloseSocket();
	return 0;
}

bool ListenOnSocket() {
	if (iProtocol == IPPROTO_UDP) { printFormatted("SERVER","Warning","UDP connection does not support listening"); return false; }
	if (sock == -1) { return false; }
	if (boundToPort == -1) { return false; }
	
	int result = listen(sock,1);
	if (result == 0) {
		printFormatted("SERVER","Log","Listening on port...");
		return true;
	}

	printFormatted("SERVER","Error","Could not listen on port: " + std::to_string(boundToPort));
	CloseSocket();
	
	return false;
}

bool AcceptClient() {
	if (sock == -1) { printFormatted("SERVER","ERROR","Server socket not initialized"); return false; }
	if (boundToPort == -1) { printFormatted("SERVER","ERROR","Server socket not bound to port"); return false; }
	
	struct sockaddr_in from;
	unsigned int fromlen = sizeof(from);
	clientSock = accept(sock, (struct sockaddr *) &from, &fromlen);

	if (clientSock < 0) {
		printFormatted("SERVER","Warning","Client accept failed");
		return false;
	}

	printFormatted("SERVER","Success","Client connected");
	return true;
}

bool CloseSocket() { return CloseSocket(sock); }
bool CloseClientSocket() { return CloseSocket(clientSock); }
bool CloseSocket(int i) {
	bool result = shutdown(i, 2)==0;
	if (result) { sock = -1; boundToPort = -1; }
	return result;
}

bool SendToClient(std::string dataToSend) {
	if (clientSock == -1) { printFormatted("SERVER","ERROR","Server socket not initialized"); return false; }
	if (boundToPort == -1) { printFormatted("SERVER","ERROR","Server socket not bound to port"); return false; }
	
	dataToSend += "\n";
	
	const char* sendBuff = dataToSend.c_str();
	send(clientSock, sendBuff, strlen(sendBuff), MSG_DONTWAIT);
	return false;
}

std::string convertToString(char* a) {
    std::string s = a;
    return s;
}


std::string GetClientResponse() {
	if (clientSock == -1) { printFormatted("SERVER","ERROR","Server socket not initialized"); return "INTERNAL ERROR"; }
	if (boundToPort == -1) { printFormatted("SERVER","ERROR","Server socket not bound to port"); return "INTERNAL ERROR"; }
	
	char* sendBuff[DEFAULT_BUFLEN];
	long timer = time(NULL);
	while(time(NULL) - timer < 5) {
		if (recv(clientSock, sendBuff, DEFAULT_BUFLEN, MSG_DONTWAIT) > 0)
			clientResponse += convertToString((char *)sendBuff);
		if (clientResponse.find("\n") != std::string::npos) {
			std::string result = clientResponse.substr(0,clientResponse.find("\n"));
			clientResponse = clientResponse.substr(
				clientResponse.find("\n")+1,
				clientResponse.length()-1
			);
			
			return result;
		}
	}
	
	return "";
}

#endif
