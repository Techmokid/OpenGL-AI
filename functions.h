#ifndef COMPILER_FUNCTIONS_LIST    // To make sure you don't declare the function more than once by including the header multiple times.
#define COMPILER_FUNCTIONS_LIST

#include <iostream>		// Required for User I/O operations
#include <string.h>
#include <cstring>
#include <chrono>			// Required for time keeping and timeout periods
#include <stdlib.h>		// Required to clear the console
#include <vector>			// Required for dynamic sized arrays (Effectively lists)
#include <math.h>			// Required for math floor function
#include <stdexcept>	// Required for error handling
#include <thread>			// Required to allow for CPU multithreading
#include <fstream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>

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
void print(double x);
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
std::string RemoveSpecificCharacter(std::string x, char delim);

// AI_Core.cpp
struct Genome_GPU {
	Genome_GPU() { }
	int ID = -1;
	double fitness = std::numeric_limits<int>::min();
	double prev_fitness = std::numeric_limits<int>::min();
	int Nodes_Start_Index = -1;
	int Nodes_End_Index = -1;
};
		
struct Node_GPU {
	Node_GPU() { }
	int ID = -1;			//Identification code		// The ID of this node
			
	int nTT = 0;		//Node trigger typeof		// 0 is step, 1 is sigmoid
	double nB = 0;		//Node bias
	double pNB = 0;		//Previous node bias
			
	bool nII = false; 	//node Is Input				// Is the node is an input or not
	bool nIO = false; 	//node Is Output			// Is the node is an output or not
	double nIV = 0;			//node Input Value			// If the node is an input, what have we entered
	double pO = -99999;	//precalculated Output		// This variable just allows for quicker genome output computing
			
	int wSI = 0; 		//weights Start Index		// This is the position in the weights array where the start of this nodes connections are held
	int wEI = 0;			//weights End Index			// This is the position in the weights array where the end of this nodes connections are held
};
		
struct NodeConnection_GPU {
	int NodePos;
	double Weight;
	double Prev_Weight;
};
		
struct Network_GPU {
	std::vector<Genome_GPU> genomes;
	std::vector<Node_GPU> nodes;
	std::vector<NodeConnection_GPU> connections;
};

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

void LoadNetworkGenomes_MTwTDC(ThreadDataContainer* TDC);
void LoadNetworkNodes_MTwTDC(ThreadDataContainer* TDC);
void LoadNetworkConnections_MTwTDC(ThreadDataContainer* TDC);
void SaveNetworkGenomes_MTwTDC		(ThreadDataContainer* TDC);
void SaveNetworkNodes_MTwTDC			(ThreadDataContainer* TDC);
void SaveNetworkConnections_MTwTDC(ThreadDataContainer* TDC);
Network_GPU* GetNetworkPointer();
std::vector<std::vector<float>> GetNetworkOutput(std::vector<float> inputs);
void GetGenomeOutput_MTwTDC(ThreadDataContainer* TDC);
float GetNodeOutput(int x);
float GetNodeOutput(Node_GPU N);
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

//OpenGL_Interface.cpp
std::string GetShaderCode(std::string shaderPath);
static void error_callback(int error, const char* description);
void StartWindow();
GLuint CompileShader(std::string computeShaderCode);
GLuint CompileShader(const char* computeShaderSource);
GLuint StartShaderProgram(GLuint computeShader);
GLuint StartShaderProgram(GLuint computeShader, std::vector<GLuint*> ssbo);
GLuint InitializeShader(std::string shaderPath);
void checkShaderCompileStatus(GLuint shader);
void ShutDownOpenGL();
