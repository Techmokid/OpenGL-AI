//Custom code setup
#include "functions.h"
#include <iostream>

// Multiple variants of saying "Stop program" so I don't have to think which I used
#define quit() 			waitForUserInput();return 0;
#define end() 			waitForUserInput();return 0;
#define stop() 			waitForUserInput();return 0;
#define quitProgram() 	waitForUserInput();return 0;
#define endProgram() 	waitForUserInput();return 0;
#define stopProgram() 	waitForUserInput();return 0;

// The port to use if using an internal server
#define PORT 3490

// Where to physically save the network
#ifdef _WIN32
//#define NEURAL_NETWORK_SAVE_LOCATION expandEnvironmentVariables("%USERPROFILE%/Desktop/AI Network")
#define NEURAL_NETWORK_SAVE_LOCATION expandEnvironmentVariables("F:/AI Network")
#else
#define NEURAL_NETWORK_SAVE_LOCATION expandEnvironmentVariables("$HOME/Desktop/AI Network")
#endif

#define GPU_AVAILABLE_CORE_COUNT 8704

GLuint CSH_RunAndTrainNetwork = -1;

// 0 - Not saving. GPU has nothing to do
// 1 - Saving. GPU is to continue training
// 2 - Saved. Awaiting GPU to aknowledge
// 3 - GPU successfully copied across data to CPU
int savingProcessStatus = 0;

int main() {
	// Set up the terminal with basic starting information
	ClearConsole();
	
	printFormatted("Info", "Log",     "This is a logged message or generic output");
	printFormatted("Info", "Debug",   "This is debugging code output");
	printFormatted("Info", "Warning", "This is a warning that a function is having troubles");
	printFormatted("Info", "Error",   "This indicates something went very wrong");
	printFormatted("Info", "Success", "This indicates that a function completed!");
			
	print(); print();
	print("---------------------------------------------------------------------");
	print(); print();
			
	printFormatted("Main", "Debug", "Good morning user! Let's get started shall we?");
	printFormatted("Main", "Debug", "Let me boot the server up for you!");
	print();
	
	StartWindow();
	printFormatted("Main", "Success", "OpenGL operational and ready");
	printFormatted("Main", "Success", "Shaders compiled");
	printFormatted("Main", "Log", "Set save location to: " + NEURAL_NETWORK_SAVE_LOCATION);
	
	CreateNewLayeredNetwork(GPU_AVAILABLE_CORE_COUNT, 365, 150, 5, 2);
	//SaveNeuralNetwork(NEURAL_NETWORK_SAVE_LOCATION);
	
	Network_GPU* NGPU = GetNetworkPointer();
	
	std::vector<float> neuralInputs;
	std::vector<std::vector<float>> neuralWallets;
	for (int i = 0; i < 365; i++) {
		neuralInputs.push_back(GetAndUpdateFakeMarketPrice());
		neuralWallets.push_back(std::vector<float>());
	}
	
	//Starting computation window
	StartWindow();
	
	//Initialize the shader
	printFormatted("OpenGL", "Log", "Initializing Shader");
	GLuint computeHandle = InitializeShader("Neural Network.shader");
	printFormatted("OpenGL", "Success", "Shader Init");
	
	//Apply the SSBO
	printFormatted("OpenGL", "Log", "Applying Shader SSBOs");
	GLuint SSBOs[8];
	glGenBuffers(8, SSBOs);
	Set_SSBO_Buffer(NGPU->genomes,     SSBOs[0] ,0);								// Set the neural genomes
	Set_SSBO_Buffer(NGPU->nodes,       SSBOs[1] ,1);								// Set the neural nodes
	Set_SSBO_Buffer(NGPU->connections, SSBOs[2] ,2);								// Set the neural connections
	Set_SSBO_Buffer(std::vector<float>(GetGenomeInputCount(),  0), SSBOs[3] ,3);	// Set the input array
	Set_SSBO_Buffer(std::vector<float>(GetGenomeOutputCount(), 0), SSBOs[4] ,4);	// Set the output array
	Set_SSBO_Buffer(0, SSBOs[5] ,5);												// Set the epoch counter to epoch 0
	Set_SSBO_Buffer(0, SSBOs[6] ,6);												// Set the ResetGenomeIDStart to 0
	Set_SSBO_Buffer(0, SSBOs[7] ,7);												// Set the GenomeResetCount to 0
	printFormatted("OpenGL", "Success", "Shader SSBOs Applied");
	
	
	
	// Everything above this statement works perfectly.
	// --------------------------------------------------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------------------------------------------------
	// Everything below this statement may need to be rewritten entirely
	
	
	
	bool already = false;
	while(true) {
		// Training loop
		float worstFitness = 0;
		float averageFitness = 0;
		float bestFitness = 0;
		
		neuralInputs.push_back(GetAndUpdateFakeMarketPrice());
		neuralInputs.erase(neuralInputs.begin());
		//std::vector<std::vector<float>> networkOutputs = GetNetworkOutput(neuralInputs);
		std::vector<float> networkFitnesses;
		for (int i = 0; i < NGPU->genomes.size(); i++) {
			float genomeFitness = getRandomFloat();
			
			
			
			
			
			
			averageFitness += genomeFitness;
			networkFitnesses.push_back(genomeFitness);
			
			worstFitness = std::min(genomeFitness, worstFitness);
			bestFitness =  std::max(genomeFitness, bestFitness);
		}
		
		averageFitness /= NGPU->genomes.size();
		
		SetNetworkFitnesses(networkFitnesses);
		printFormatted("Main", "Log", "Epoch: " + std::to_string(getCurrentEpoch()));
		printFormatted("Main", "Log", "Worst Fitness: " + std::to_string(worstFitness));
		printFormatted("Main", "Log", "Average Fitness: " + std::to_string(averageFitness));
		printFormatted("Main", "Log", "Best Fitness: " + std::to_string(bestFitness));
		print();
		
		if (savingProcessStatus == 0) {
			already = false;
			savingProcessStatus = 1;
			SaveNeuralNetworkNonBlocking(NEURAL_NETWORK_SAVE_LOCATION, &savingProcessStatus);
		} else if ((savingProcessStatus == 2) && (already == false)) {
			already = true;
			printFormatted("Main","Debug","Saving process completed");
			while(true);
		} else if (savingProcessStatus == 3) {
			
		}
	}
}
