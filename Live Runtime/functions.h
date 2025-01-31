#ifndef COMPILER_FUNCTIONS_LIST    // To make sure you don't declare the function more than once by including the header multiple times.
#define COMPILER_FUNCTIONS_LIST

//#define OS_GUI_PRESENT

#include <iostream>		// Required for User I/O operations
#include <string.h>
#include <cstring>
#include <cstdint>      // Required for uint64_t support
#include <chrono>		// Required for time keeping and timeout periods
#include <stdlib.h>		// Required to clear the console
#include <vector>		// Required for dynamic sized arrays (Effectively lists)
#include <math.h>		// Required for math floor function
#include <stdexcept>	// Required for error handling
#include <thread>		// Required to allow for CPU multithreading
#include <fstream>
#include <filesystem>

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
//void print(auto x);
void print(int x);
void print(float x);
void print(double x);
void print(uint64_t x);
void print(std::string x);
void SilenceConsoleOutput(bool silence);
void printFormatted(std::string module, std::string status, std::string msg);
void printSuccess(std::string module, std::string msg);
void printError(std::string module, std::string msg);
void printLog(std::string module, std::string msg);
void printInfo(std::string module, std::string msg);
void printDebug(std::string module, std::string msg);
void SetConsoleColor(int colour);
std::string TimeFormatter(double x);
std::string DataSizeFormatter(uint64_t x);
void waitForUserInput();
std::string stringToLower(std::string in);
std::string stringToLowerCapitalized(std::string in);
std::string stringToUpper(std::string in);
std::vector<std::string> SplitString(std::string inputVal, char delimiter);
std::string ASCII_To_Numeric(std::string x);
std::string RemoveSpecificCharacter(std::string x, char delim);
std::string expandEnvironmentVariables(const std::string& inputPath);
void quit();
void delay(uint64_t t);
void SetWallpaper(std::string path);

// AI_Core.cpp
struct Genome_GPU {
	Genome_GPU() { }
	uint64_t ID = -1;
	float fitness = std::numeric_limits<uint64_t>::min();
	float prev_fitness = std::numeric_limits<uint64_t>::min();
	uint64_t Nodes_Start_Index = -1;
	uint64_t Nodes_End_Index = -1;
	uint64_t FailedNetworkIterations = 0;
};
		
struct Node_GPU {
	Node_GPU() { }
	uint64_t ID = -1;			//Identification code		// The ID of this node
			
	uint64_t nTT = 0;		//Node trigger typeof		// -1 is disabled node, 0 -> 16 is different activation functions
	float nB = 0;		//Node bias
	float pNB = 0;		//Previous node bias
			
	bool nII = false; 	//node Is Input				// Is the node is an input or not
	bool nIO = false; 	//node Is Output			// Is the node is an output or not
	float nIV = 0;			//node Input Value			// If the node is an input, what have we entered
	float pO = -99999;	//precalculated Output		// This variable just allows for quicker genome output computing
			
	uint64_t wSI = 0; 		//weights Start Index		// This is the position in the weights array where the start of this nodes connections are held
	uint64_t wEI = 0;			//weights End Index			// This is the position in the weights array where the end of this nodes connections are held
};
		
struct NodeConnection_GPU {
	uint64_t NodePos;
	float Weight;
	float Prev_Weight;
};
		
struct Network_GPU {
	std::vector<Genome_GPU> genomes;
	std::vector<Node_GPU> nodes;
	std::vector<NodeConnection_GPU> connections;
	
    Network_GPU clone() const {
        Network_GPU clonedNetwork = Network_GPU(); // Uses the copy constructor
		clonedNetwork.genomes = this->genomes;
        clonedNetwork.nodes = this->nodes;
        clonedNetwork.connections = this->connections;
		return clonedNetwork;
    }
};

struct ThreadDataContainer {
	bool threadCompletionStatus = false;
	uint64_t ID;
	std::string path;
	uint64_t EndIndex;
};

void ForceNetworkCopyMatching();
Network_GPU* GetNetworkPointer();

uint64_t GetGenomeCount();
uint64_t GetGenomeInputCount();
uint64_t GetGenomeNodesPerLayer();
uint64_t GetGenomeHiddenLayerCount();
uint64_t GetGenomeOutputCount();
uint64_t getCurrentEpoch();

void CreateNewLayeredNetwork(uint64_t genomeCount, uint64_t inputNodes, uint64_t nodesPerLayer, uint64_t hiddenLayerCount, uint64_t outputNodes);

void LoadNetworkGPU(std::string dir);
void LoadNetworkGPU();

void SaveNeuralNetworkNonBlocking(std::string dir, bool* status);
void SaveNeuralNetworkNonBlocking(bool* status);
void SaveNeuralNetworkHandler(bool* status);
void SaveNeuralNetwork();
void SaveNeuralNetwork(std::string dir);
void SaveNeuralNetworkInternal(std::string dir);
void SetCPUSaveThreadCount(uint64_t x);

float getRandomFloat();
float getRandomFloat(float HI, float LO);

void LoadNetworkGenomes_MTwTDC(ThreadDataContainer* TDC);
void LoadNetworkNodes_MTwTDC(ThreadDataContainer* TDC);
void LoadNetworkConnections_MTwTDC(ThreadDataContainer* TDC);
void SaveNetworkGenomes_MTwTDC(ThreadDataContainer* TDC);
void SaveNetworkNodes_MTwTDC(ThreadDataContainer* TDC);
void SaveNetworkConnections_MTwTDC(ThreadDataContainer* TDC);
Network_GPU* GetNetworkPointer();

std::vector<std::vector<float>> GetNetworkOutput(GLuint location);
void GetGenomeOutput_MTwTDC(ThreadDataContainer* TDC);
float GetNodeOutput(uint64_t x);
float GetNodeOutput(Node_GPU N);
float GetRandomFloat(float min, float max);
void TrainGenome_MTwTDC(ThreadDataContainer* TDC);
void TrainNetwork();
void SetNetworkFitnesses(std::vector<float> fitnesses);
std::vector<std::vector<float>> GetNetworkOutput(GLuint x);

// Socket.cpp
bool isConnectedToClient();
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

//FakeMarket.cpp
void UpdateFakeMarket();
float GetFakeMarketPrice();
std::vector<float> GetFakeMarketHistory();
float GetAndUpdateFakeMarketPrice();

//NetworkAnomolyTests.cpp
void RunTests();

//OpenGL_Interface.cpp
std::string GetShaderCode(std::string shaderPath);
void StartWindow();
GLuint CompileShader(std::string computeShaderCode);
GLuint CompileShader(const char* computeShaderSource);
GLuint StartShaderProgram(GLuint computeShader);
GLuint InitializeShader(std::string shaderPath);
void checkShaderCompileStatus(GLuint shader);
void ShutDownOpenGL();

template<typename T>
void Set_SSBO_Buffer(std::vector<T> &obj, GLuint ssbo, uint64_t buffer_index) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, obj.size() * sizeof(T), obj.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, buffer_index, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
template<typename T>
void Set_SSBO_Buffer(T &obj, GLuint ssbo, uint64_t buffer_index) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T), &obj, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, buffer_index, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
template<typename T>
void Set_SSBO_Buffer(T obj, GLuint ssbo, uint64_t buffer_index) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T), &obj, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, buffer_index, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
template<typename T>
void Get_SSBO_Buffer(std::vector<T> &obj, GLuint ssbo) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, obj.size() * sizeof(T), obj.data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
template<typename T>
void Get_SSBO_Buffer(T &obj, GLuint ssbo) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(T), obj.data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
template<typename T>
void Edit_SSBO_Buffer(std::vector<T> &obj, GLuint ssbo) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, obj.size() * sizeof(T), obj.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
template<typename T>
void Edit_SSBO_Buffer(T &obj, GLuint ssbo) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, obj.size() * sizeof(T), obj.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
template<typename T>
void Edit_SSBO_Buffer(T obj, GLuint ssbo) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(T), &obj);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}