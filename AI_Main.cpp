//Custom code setup
#include "functions.h"
#include <iostream>

#define quit() 	waitForUserInput();return 0;
#define end() 		waitForUserInput();return 0;
#define stop() 	waitForUserInput();return 0;
#define quitProgram() 	waitForUserInput();return 0;
#define endProgram() 	waitForUserInput();return 0;
#define stopProgram() 	waitForUserInput();return 0;

#define PORT 3490

GLuint CSH_GetNetworkOutput = -1;
GLuint CSH_TraingNetwork = -1;

int main() {
	// This is how to change the terminal title. I have no clue how this works, but StackOverflow does!
	// PS1='\[\e]0;Neural Server\a\]${debian_chroot:+($debian_chroot)}\u@\h:\w\$ '
	
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
	
	// Try to start up the server
	if (StartSocket(PORT) < 0) {
		printFormatted("MAIN","Error","Failed to start server socket!");
		quitProgram();
	}
	
	printFormatted("MAIN","Success","Started server socket");
	if (!AcceptClient()) {
		printFormatted("MAIN","Error","Failed to accept client!");
		quitProgram();
	}
	
	while(true) {
		std::string msgRaw = GetClientResponse();
		
		try {
			if (msgRaw.length() > 0) {
				msgRaw = RemoveSpecificCharacter(msgRaw,' ');
				std::string command = stringToLower(SplitString(msgRaw,':')[0]);
				std::vector<std::string> msgParams = SplitString(SplitString(msgRaw,':')[1],',');
				//ChkSum = 1000*(Sqrt(Inputs)+Sqrt(HiddenLayers))/(Sqrt(HiddenNodesPerLayer + Outputs) + Sqrt(Outputs))
				
				if (command == "generatenewnetwork") {
					printFormatted("MAIN", "Log", "Received New Network Generation Command");
					
					int genomes=-1,inputs=-1,outputs=-1,hiddenLayers=-1,nodesPerLayer=-1;
					for (int i = 0; i < msgParams.size(); i++) {
						msgParams[i] = stringToLower(msgParams[i]);
						std::vector<std::string> param = SplitString(msgParams[i],'=');
						std::string key = param[0];
						int val = std::stoi(param[1].c_str());
						
						if (key == "genomes");
							genomes = val;
						if (key == "inputs")
							inputs = val;
						if (key == "outputs")
							outputs = val;
						if (key == "hiddenlayers")
							hiddenLayers = val;
						if (key == "nodesperlayer")
							nodesPerLayer = val;
					}
					
					std::vector<std::string> errMsg;
					if (genomes == -1)
						errMsg.push_back("genomes");
					if (inputs == -1)
						errMsg.push_back("inputs");
					if (outputs == -1)
						errMsg.push_back("outputs");
					if (hiddenLayers == -1)
						errMsg.push_back("hiddenLayers");
					if (nodesPerLayer == -1)
						errMsg.push_back("nodesPerLayer");
					if (errMsg.size() > 0) {
						SendToClient("Error: Missing the following parameters:");
						for (int i = 0; i < errMsg.size(); i++) {
							SendToClient(" - " + errMsg[i]);
						}
					} else {
						SendToClient("Generated New Network With the Following Local Parameters: ");
						SendToClient(" - Genomes: " + std::to_string(genomes));
						SendToClient(" - Inputs:  " + std::to_string(inputs));
						SendToClient(" - Outputs: " + std::to_string(outputs));
						SendToClient(" - Layers:  " + std::to_string(hiddenLayers));
						SendToClient(" - Nodes Per Layer: " + std::to_string(nodesPerLayer));
						CreateNewLayeredNetwork(genomes, inputs, nodesPerLayer, hiddenLayers, outputs);
						
						Network_GPU* NGPU = GetNetworkPointer();
						SendToClient("Generated New Network With the Following Global Parameters: ");
						SendToClient(" - Total Genome Count: " + std::to_string(NGPU->genomes.size()));
						SendToClient(" - Nodes per Genome:   " + std::to_string(NGPU->nodes.size() / NGPU->genomes.size()));
						SendToClient(" - Total Node Count:   " + std::to_string(NGPU->nodes.size()));
						SendToClient(" - Total Connections Count: " + std::to_string(NGPU->connections.size()));
						
						int gDiskUse = NGPU->genomes.size()*sizeof(Genome_GPU);
						int nDiskUse = NGPU->nodes.size()*sizeof(Node_GPU);
						int cDiskUse = NGPU->connections.size()*sizeof(NodeConnection_GPU);
						int totalDiskUse = gDiskUse + nDiskUse + cDiskUse;
						SendToClient(" - Theoretical disk usage:  " + std::to_string(totalDiskUse));
					}
				} else if (command == "loadnetwork") {
					printFormatted("MAIN", "Log", "Received Load Network Command");
					LoadNetworkGPU("/home/andrey/Desktop/AI Network");
				} else if (command == "savenetwork") {
					printFormatted("MAIN", "Log", "Received Save Network Command");
					SaveNeuralNetwork("/home/andrey/Desktop/AI Network");
				} else if (command == "getoutput") { //WIP
					printFormatted("MAIN", "Log", "Received Neural Output Retrieval Command");
					
					
					
					
					
					
					
				} else if (command == "ping") {
					printFormatted("MAIN", "Log", "Received Ping");
					SendToClient("Pong");
				} else {
					printFormatted("MAIN", "Log", "Unknown message detected");
					SendToClient("Unknown command");
				}
			} else {
				printFormatted("MAIN", "Log", "Client Timeout");
			}
		} catch (const std::exception& e) {
			printFormatted("MAIN", "Log", "Error while parsing client response");
			SendToClient("Malformed request: " + msgRaw);
		}
	}
	
	// Finally shut down the server	
	CloseClientSocket();
	printFormatted("MAIN","Success","Closed the client socket");
	print("Press any key to quit");
	endProgram();
}
