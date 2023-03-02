#ifndef COMPILER_FUNCTIONS_LIST    // To make sure you don't declare the function more than once by including the header multiple times.
#define COMPILER_FUNCTIONS_LIST

#include <iostream>		// Required for User I/O operations
#include <cstring>
#include <chrono>			// Required for time keeping and timeout periods
#include <stdlib.h>		// Required to clear the console
#include <vector>			// Required for dynamic sized arrays (Effectively lists)
#include <math.h>			// Required for math floor function
#include <stdexcept>	// Required for error handling
#include <thread>			// Required to allow for CPU multithreading
#include <fstream>

#ifdef _WIN32
#include <winsock2.h>	// Required for server socket connections
#include <windows.h>	// Required for console text colour editing
#else
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

// Helper_Functions.cpp
void ClearConsole();
void print();
void print(int x);
void print(float x);
void print(long unsigned int x);
void print(std::string x);
void printFormatted(std::string module, std::string status, std::string msg);
void printSuccess(std::string module, std::string msg);
void printError(std::string module, std::string msg);
void printLog(std::string module, std::string msg);
void printInfo(std::string module, std::string msg);
void printDebug(std::string module, std::string msg);
void SetConsoleColor(int colour);
std::string TimeFormatter(double x);
void waitForUserInput();
std::string stringToLower(std::string in);
std::string stringToLowerCapitalized(std::string in);
std::string stringToUpper(std::string in);
std::vector<std::string> SplitString(std::string inputVal, char delimiter);
std::string ASCII_To_Numeric(std::string x);

// AI_Core.cpp
struct ThreadDataContainer {
	bool threadCompletionStatus = false;
	int ID;
	std::string path;
	int EndIndex;
};

void CreateNewLayeredNetwork(int genomeCount, int inputNodes, int nodesPerLayer, int hiddenLayerCount, int outputNodes);
void quit();

void LoadNetworkGPU(std::string dir);
void LoadNetworkGPU();

void SaveNeuralNetwork();
void SaveNeuralNetwork(std::string dir);
void SaveNeuralNetworkInternal(std::string dir);
void SetCPUSaveThreadCount(int x);

float getRandomFloat();
float getRandomFloat(float HI, float LO);

void SaveNetworkGenomes_MTwTDC		(ThreadDataContainer* TDC);
void SaveNetworkNodes_MTwTDC			(ThreadDataContainer* TDC);
void SaveNetworkConnections_MTwTDC(ThreadDataContainer* TDC);

// Socket.cpp
int StartSocket(int port);
bool OpenSocket();
bool CloseSocket();
#ifdef _WIN32
bool CloseSocket(SOCKET i);
#else
bool CloseSocket(int i);
#endif
bool CloseClientSocket();
bool BindSocket(int port);
bool ListenOnSocket();
bool AcceptClient();
std::string GetClientResponse();
bool SendToClient(std::string dataToSend);
#endif
