#include "functions.h"
#include <vector>
#include <filesystem> // Required to do file management

int numberOfIndexesPerThread = 10000;

const int numberOfAvailableActivationFunctions = 2;

// Struct definitions
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
			
	double nTT = 0;		//Node trigger typeof		// 0 is step, 1 is sigmoid
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

// Variable declarations
Network_GPU* NGPU = new Network_GPU();
int maxThreadCount = 8;
std::string saveDirectory = "";

// Function declarations
float getRandomFloat() { return getRandomFloat(0,1); }
float getRandomFloat(float HI, float LO) {
	return LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
}

void CreateNewLayeredNetwork(int genomeCount, int inputNodes, int nodesPerLayer, int hiddenLayerCount, int outputNodes) {
	srand(time(NULL));
	
	int nodeCountPerGenome = inputNodes + nodesPerLayer*hiddenLayerCount + outputNodes;
	int connCountPerGenome = inputNodes*nodesPerLayer + hiddenLayerCount*(hiddenLayerCount-1) + outputNodes*nodesPerLayer;
			
	printFormatted("Neural","Debug","New GPU Network Data:");
	printFormatted("Neural","Debug"," - Genome Count: " + std::to_string(genomeCount));
	printFormatted("Neural","Debug"," - Nodes Per Genome: " + std::to_string(nodeCountPerGenome));
	printFormatted("Neural","Debug"," - Node Connections Per Genome: " + std::to_string(connCountPerGenome));
	printFormatted("Neural","Debug"," - Nodes Total: " + std::to_string(nodeCountPerGenome*genomeCount));
	printFormatted("Neural","Debug"," - Connections Total: " + std::to_string(connCountPerGenome*genomeCount));
	
	delete NGPU;
	NGPU = new Network_GPU();
	int totalNodeCountPerGenome = (inputNodes + nodesPerLayer*hiddenLayerCount + outputNodes);
	int totalNodeConnectionsCountPerGenome = (inputNodes*nodesPerLayer + nodesPerLayer*nodesPerLayer*hiddenLayerCount + outputNodes*nodesPerLayer) * genomeCount;
	
	NGPU->genomes.clear();
	NGPU->genomes.resize(genomeCount);
	for(int i = 0; i < genomeCount; i++) {
		NGPU->genomes[i].ID = i;
		NGPU->genomes[i].fitness = std::numeric_limits<int>::min();
		NGPU->genomes[i].prev_fitness = std::numeric_limits<int>::min();
		NGPU->genomes[i].Nodes_Start_Index = totalNodeCountPerGenome * i;
		NGPU->genomes[i].Nodes_End_Index = totalNodeCountPerGenome * (i+1) - 1;
	}
	
	NGPU->connections.clear();
	NGPU->connections.resize(totalNodeConnectionsCountPerGenome);
	
	int tempWeightsCount = 0;
	NGPU->nodes.clear();
	NGPU->nodes.resize(totalNodeCountPerGenome * genomeCount);
	
	for (int i = 0; i < NGPU->nodes.size(); i++) {
		NGPU->nodes[i].ID = i;
		//NGPU->nodes[i].nTT = 1;	//Node trigger type. 0 for step, 1 for sigmoid
		NGPU->nodes[i].nTT = rand() % numberOfAvailableActivationFunctions;
		
		NGPU->nodes[i].nB = getRandomFloat();	//Node Bias
		NGPU->nodes[i].pNB = NGPU->nodes[i].nB;
				
		int startOfGenome = (int)(totalNodeCountPerGenome * floor((double)NGPU->nodes[i].ID / (double)totalNodeCountPerGenome));
		NGPU->nodes[i].nII = i < startOfGenome + inputNodes;
		NGPU->nodes[i].nIO = i >= startOfGenome + inputNodes + nodesPerLayer*hiddenLayerCount;
				
		//NGPU.nodes[i].nIV = 0;
		//NGPU.nodes[i].pO = -99999;
		NGPU->nodes[i].wSI = tempWeightsCount;
				
		int startOfNonInputs = startOfGenome + inputNodes;
		int numConnections = nodesPerLayer;
		int startOfLastLayer = startOfGenome;
		int currentLayer = std::floor(((i - startOfGenome) - inputNodes)/nodesPerLayer) + 1;
		
		if (i - startOfGenome >= inputNodes) {
			if (i - startOfGenome < totalNodeCountPerGenome - outputNodes) {
				// Node is hidden node
				if (currentLayer > 1)
					startOfLastLayer = (currentLayer - 2)*nodesPerLayer + inputNodes + startOfGenome;
			} else {
				// Node is output node
				startOfLastLayer = startOfGenome + totalNodeCountPerGenome - outputNodes - nodesPerLayer;
			}
		}
		
		if (NGPU->nodes[i].nIO) {
			NGPU->nodes[i].wEI = tempWeightsCount + numConnections - 1;
					
			int firstIndex = startOfGenome + totalNodeCountPerGenome - outputNodes - nodesPerLayer;
			for (int x = 0; x < numConnections; x++) {
				int pos = tempWeightsCount + x;
				NGPU->connections[pos].NodePos = firstIndex + x;
				NGPU->connections[pos].Weight = getRandomFloat();
				NGPU->connections[pos].Prev_Weight = NGPU->connections[pos].Weight;
			}
					
			tempWeightsCount += numConnections;
		} else if (!(NGPU->nodes[i].nII)) {
			bool isSecondLayer = i < startOfGenome + inputNodes + nodesPerLayer;
			if (isSecondLayer) { numConnections = inputNodes; }
			NGPU->nodes[i].wEI = tempWeightsCount + numConnections - 1;
			tempWeightsCount += numConnections;
					
			int startPoint = startOfLastLayer;
			if (isSecondLayer) {
				startPoint = startOfGenome;
				for (int x = 0; x < numConnections; x++) {
					int pos = NGPU->nodes[i].wSI + x;
					NGPU->connections[pos].NodePos = startPoint + x;
					NGPU->connections[pos].Weight = getRandomFloat();
					NGPU->connections[pos].Prev_Weight = NGPU->connections[pos].Weight;
				}
			}
		}
	}
	
	// Now just check to make sure the network has no circular definitions and all nodes are ID'd
	int trimmableGenomes = 0;
	for (int i = 0; i < NGPU->genomes.size(); i++) {
		if (NGPU->genomes[i].ID == -1) { trimmableGenomes++; }
		if ((NGPU->genomes[i].Nodes_Start_Index > (int)NGPU->nodes.size() + 1) || (NGPU->genomes[i].Nodes_End_Index < 0)) {
			printFormatted("Main","Error","While creating neural net. Genome indexing failure");
			quit();
		}
	}
			
	int trimmableNodes = 0;
	for (int i = 0; i < NGPU->nodes.size(); i++) {
		if (NGPU->nodes[i].ID == -1) { trimmableNodes++; }
		if ((NGPU->nodes[i].wEI > (int)NGPU->connections.size() + 1) || (NGPU->nodes[i].wSI < 0)) {
			printFormatted("Main","Error","While creating neural net. Node indexing failure");
			quit();
		}
	}
	
	for (int i = 0; i < NGPU->connections.size(); i++) {
		if ((NGPU->connections[i].NodePos > NGPU->nodes.size()) || (NGPU->connections[i].NodePos < 0)) {
			printFormatted("Main","Error","While creating neural net. Connection indexing failure");
			quit();
		}
	}
	
	if (trimmableGenomes > 0) { throw std::invalid_argument(std::to_string(trimmableGenomes) + " genomes found with unset ID"); }
	if (trimmableNodes > 0) { throw std::invalid_argument(std::to_string(trimmableNodes) + " nodes found with unset ID"); }
	
}

void quit() {
	print();
	print();
	print("Press enter to close the program");
	std::cin.get();
	exit(1);
}

void SaveNeuralNetwork(std::string dir) { saveDirectory = dir + "/"; SaveNeuralNetwork(); }
void SaveNeuralNetwork() {
	if (saveDirectory == "") {
		printFormatted("Save","Error","Could not save GPU network. SaveNetworkGPU was called without setting save directory");
		quit();
	}
	
	std::string netSaveDir = saveDirectory + "Network Save Orig/";
	std::string netBackupDir = saveDirectory + "Network Save Backup/";
	std::string netStatusFileDir = saveDirectory + "saveStatus.txt";
	
	if (! std::filesystem::is_directory(saveDirectory)) {
		std::filesystem::create_directories(saveDirectory);
	}
	
	//Save the original copy
	std::fstream netStatusFile;
	netStatusFile.open(netStatusFileDir, std::fstream::out | std::fstream::trunc);
	netStatusFile << "1";
	netStatusFile.close();
	
	if (std::filesystem::is_directory(netSaveDir)) {
		printFormatted("Save","Log","Clearing old primary save");
		std::filesystem::remove_all(netSaveDir);
		printFormatted("Save","Success","Cleared old primary save");
	}
	printFormatted("Save","Log","Saving AI network - primary copy");
	std::filesystem::create_directories(netSaveDir);
	SaveNeuralNetworkInternal(netSaveDir);
	printFormatted("Save","Success","Saved AI network - primary copy");
	
	//Save the backup copy
	netStatusFile.open(netStatusFileDir, std::fstream::out | std::fstream::trunc);
	netStatusFile << "2";
	netStatusFile.close();
	if (std::filesystem::is_directory(netBackupDir)) {
		printFormatted("Save","Log","Clearing old backup save");
		std::filesystem::remove_all(netBackupDir);
		printFormatted("Save","Log","Cleared old backup save");
	}
	printFormatted("Save","Log","Saving AI network - backup copy");
	std::filesystem::create_directories(netBackupDir);
	SaveNeuralNetworkInternal(netBackupDir);
	printFormatted("Save","Success","Saved AI network - backup copy");
		
	//Now finish up
	netStatusFile.open(netStatusFileDir, std::fstream::out | std::fstream::trunc);
	netStatusFile << "0";
	netStatusFile.close();
	printFormatted("Save","Success","Saved AI network!");
}

void SaveNetworkGenomes_MTwTDC(ThreadDataContainer* TDC) {
	Genome_GPU G[TDC->EndIndex - TDC->ID + 1];
	
	for(int i = TDC->ID; i <= TDC->EndIndex; i++) {
		G[i - TDC->ID] = NGPU->genomes[i];
	}
	
	std::string filepath = TDC->path + "Genomes/Genome Data " + std::to_string(TDC->ID) + ".csv";
	
	std::fstream f;
	f.open(filepath, std::fstream::out | std::fstream::trunc);
	for (int i = 0; i < sizeof(G) / sizeof(G[0]); i++) {
		// Convert the neural genome to CSV format
		std::string line = std::to_string(G[i].ID) + ",";
		line += std::to_string(G[i].fitness) + ",";
		line += std::to_string(G[i].prev_fitness) + ",";
		line += std::to_string(G[i].Nodes_Start_Index) + ",";
		line += std::to_string(G[i].Nodes_End_Index);
		f << line << std::endl;
	}
	f.close();
	
	TDC->threadCompletionStatus = true;
}

// W.I.P WIP
void SaveNetworkNodes_MTwTDC(ThreadDataContainer* TDC) {
	Node_GPU N[TDC->EndIndex - TDC->ID + 1];
	for(int i = TDC->ID; i <= TDC->EndIndex; i++) {
		N[i - TDC->ID] = NGPU->nodes[i];
	}
	
	std::string filepath = TDC->path + "Nodes/Node Data " + std::to_string(TDC->ID) + ".csv";
	std::fstream f;
	f.open(filepath, std::fstream::out | std::fstream::trunc);
	for (int i = 0; i < sizeof(N) / sizeof(N[0]); i++) {
		// Convert the neural node to CSV format
		std::string line = std::to_string(N[i].ID) + ",";
		line += std::to_string(N[i].nTT) + ",";
		line += std::to_string(N[i].nB) + ",";
		line += std::to_string(N[i].pNB) + ",";
		line += std::to_string(N[i].nII) + ",";
		line += std::to_string(N[i].nIO) + ",";
		line += std::to_string(N[i].nIV) + ",";
		line += std::to_string(N[i].pO) + ",";
		line += std::to_string(N[i].wSI) + ",";
		line += std::to_string(N[i].wEI);
		f << line << std::endl;
	}
	f.close();
	
	TDC->threadCompletionStatus = true;
}

// W.I.P WIP
void SaveNetworkConnections_MTwTDC(ThreadDataContainer* TDC) {
	NodeConnection_GPU C[TDC->EndIndex - TDC->ID + 1];
	for(int i = TDC->ID; i <= TDC->EndIndex; i++) {
		C[i - TDC->ID] = NGPU->connections[i];
	}
	
	std::string filepath = TDC->path + "Node Connections/Connection Data " + std::to_string(TDC->ID) + ".csv";
	std::fstream f;
	f.open(filepath, std::fstream::out | std::fstream::trunc);
	for (int i = 0; i < sizeof(C) / sizeof(C[0]); i++) {
		// Convert the neural connection to CSV format
		std::string line = std::to_string(C[i].NodePos) + ",";
		line += std::to_string(C[i].Weight) + ",";
		line += std::to_string(C[i].Prev_Weight);
		f << line << std::endl;
	}
	f.close();
	
	TDC->threadCompletionStatus = true;
}

void SetCPUSaveThreadCount(int x) { numberOfIndexesPerThread = x; }

void SaveNeuralNetworkInternal(std::string dir) {
	auto start = std::chrono::high_resolution_clock::now();
	
	std::filesystem::remove_all(dir + "Genomes/");
	std::filesystem::remove_all(dir + "Nodes/");
	std::filesystem::remove_all(dir + "Node Connections/");
	std::filesystem::create_directories(dir + "Genomes/");
	std::filesystem::create_directories(dir + "Nodes/");
	std::filesystem::create_directories(dir + "Node Connections/");
	
	std::vector<ThreadDataContainer*> TDC_List;
	bool run = true;
	int genomeIndex = 0;
	int maxThreadCount = std::thread::hardware_concurrency();
	int currentThreadCount = 0;
			
	printFormatted("Save","Debug", "Genome File Count: " + std::to_string(ceil((float)NGPU->genomes.size()/(float)numberOfIndexesPerThread)));
	printFormatted("Save","Debug", "Node File Count: " + std::to_string(ceil((float)NGPU->nodes.size()/(float)numberOfIndexesPerThread)));
	printFormatted("Save","Debug", "Node Connections File Count: " + std::to_string(ceil((float)NGPU->connections.size()/(float)numberOfIndexesPerThread)));
	printFormatted("Save","Debug", "Index Count per CPU Thread: " + std::to_string(numberOfIndexesPerThread));	
	
	//Now we actually start saving the AI to disk
	bool genomesComplete = false;
	bool nodesComplete = false;
	while(run) {
		while(currentThreadCount < maxThreadCount) {
			if (genomeIndex >= NGPU->connections.size()) { run = false; break; }
			
			ThreadDataContainer* TDC_Connection = new ThreadDataContainer();
			TDC_Connection->ID = genomeIndex;
			TDC_Connection->EndIndex = genomeIndex + std::min(numberOfIndexesPerThread - 1, (int)NGPU->connections.size() - genomeIndex - 1);
			
			if (TDC_Connection->EndIndex >= NGPU->connections.size()) {
				printFormatted("Save","Error", "Connection indexing error!");
				quit();
			}
			
			TDC_Connection->path = dir;
			TDC_List.push_back(TDC_Connection);
			
			std::thread connectionThread(SaveNetworkConnections_MTwTDC, TDC_Connection);
			connectionThread.detach();
			
			currentThreadCount++;
			
			if (genomeIndex < NGPU->nodes.size()) {
				ThreadDataContainer* TDC_Node = new ThreadDataContainer();
				TDC_Node->ID = genomeIndex;
				TDC_Node->EndIndex = genomeIndex + std::min(numberOfIndexesPerThread - 1,(int)NGPU->nodes.size()-genomeIndex-1);
				TDC_Node->path = dir;
				
				if (TDC_Node->EndIndex >= NGPU->nodes.size()) {
					printFormatted("Save","Error","Node indexing error!");
					quit();
				}
				
				TDC_List.push_back(TDC_Node);
				std::thread nodeThread(SaveNetworkNodes_MTwTDC, TDC_Node);
				nodeThread.detach();
				
				currentThreadCount++;
			} else {
				if (!nodesComplete) {
					nodesComplete = true;
					printFormatted("Save","Debug","Completed saving Nodes (2/3)");
				}
			}
			
			if (genomeIndex < NGPU->genomes.size()) {
				ThreadDataContainer* TDC_Genome = new ThreadDataContainer();
				TDC_Genome->ID = genomeIndex;
				TDC_Genome->EndIndex = genomeIndex + std::min(numberOfIndexesPerThread - 1,(int)(NGPU->genomes.size())-genomeIndex-1);
				TDC_Genome->path = dir;
				
				if (TDC_Genome->EndIndex >= NGPU->genomes.size()) {
					printFormatted("Save","Error","Genome indexing error!");
					quit();
				}
				
				TDC_List.push_back(TDC_Genome);
				std::thread genomeThread(SaveNetworkGenomes_MTwTDC, TDC_Genome);
				genomeThread.detach();
				
				currentThreadCount++;
			} else {
				if (!genomesComplete) {
					genomesComplete = true;
					printFormatted("Save","Debug","Completed saving Genomes (1/3)");
				}
			}
			
			genomeIndex += numberOfIndexesPerThread;
		}
		
		if (run) {
			while (currentThreadCount >= maxThreadCount) {
				for (int x = 0; x < TDC_List.size(); x++) {
					if (TDC_List[x]->threadCompletionStatus) {
						TDC_List.erase(TDC_List.begin() + x);
						currentThreadCount--;
						x--;
					}
				}
			}
		}
	}
	
	while(TDC_List.size() > 0) {
		for (int x = 0; x < TDC_List.size(); x++) {
			if (TDC_List[x]->threadCompletionStatus) {
				TDC_List.erase(TDC_List.begin() + x);
				currentThreadCount--;
				x--;
			}
		}
	}
	
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	printFormatted("Save","Log","Real time taken to save network: " + TimeFormatter(duration.count()));
}
