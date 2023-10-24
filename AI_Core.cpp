#include "functions.h"
#include <vector>

int numberOfIndexesPerThread = 10000;
int numberOfAvailableActivationFunctions = 17;

// Variable declarations
Network_GPU* NGPU = new Network_GPU();
Network_GPU* NGPU_Copy = new Network_GPU();

std::vector<std::vector<float>>* NetworkOutputs = new std::vector<std::vector<float>>();
int maxThreadCount = 4;
std::string saveDirectory = "";

int _genomeCount = 0, _inputCount = 0, _nodesPerLayer = 0, _hiddenLayerCount = 0, _outputNodes = 0, epoch = 0;
bool* savingInProgress = new bool(false);


Network_GPU* GetNetworkPointer() { return NGPU; }
int GetGenomeCount() {            return _genomeCount; }
int GetGenomeInputCount() {       return _inputCount; }
int GetGenomeNodesPerLayer() {    return _nodesPerLayer; }
int GetGenomeHiddenLayerCount() { return _hiddenLayerCount; }
int GetGenomeOutputCount() {      return _outputNodes; }
int getCurrentEpoch() {           return epoch; }

bool initiailizedRandomNumberGenerator = false;
float getRandomFloat() { return getRandomFloat(0,1); }
float getRandomFloat(float HI, float LO) {
	if (!initiailizedRandomNumberGenerator) {
		srand((unsigned)time(NULL));
		initiailizedRandomNumberGenerator = true;
	}
	
	return LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
}

void CreateNewLayeredNetwork(int genomeCount, int inputNodes, int nodesPerLayer, int hiddenLayerCount, int outputNodes) {
	print();
	printFormatted("Neural", "Log", "Generating new layered neural network");
	srand(time(NULL));
	
	int nodeCountPerGenome = inputNodes + nodesPerLayer*hiddenLayerCount + outputNodes;
	int connCountPerGenome = inputNodes*nodesPerLayer + hiddenLayerCount*(hiddenLayerCount-1) + outputNodes*nodesPerLayer;
			
	printFormatted("Neural","Debug","New GPU Network Data:");
	printFormatted("Neural","Debug"," - Genome Count: " + std::to_string(genomeCount));
	printFormatted("Neural","Debug"," - Nodes Per Genome: " + std::to_string(nodeCountPerGenome));
	printFormatted("Neural","Debug"," - Node Connections Per Genome: " + std::to_string(connCountPerGenome));
	printFormatted("Neural","Debug"," - Nodes Total: " + std::to_string(nodeCountPerGenome*genomeCount));
	printFormatted("Neural","Debug"," - Connections Total: " + std::to_string(connCountPerGenome*genomeCount));
	
	unsigned long long int gRAM = genomeCount*sizeof(Genome_GPU);
	unsigned long long int nRAM = nodeCountPerGenome*genomeCount*sizeof(Node_GPU);
	unsigned long long int cRAM = connCountPerGenome*genomeCount*sizeof(NodeConnection_GPU);
	printFormatted("Neural", "Debug", "Memory Requirements:");
	printFormatted("Neural", "Debug", " - Genome RAM: " + DataSizeFormatter(sizeof(Genome_GPU)));
	printFormatted("Neural", "Debug", " - Node RAM: " + DataSizeFormatter(sizeof(Node_GPU)));
	printFormatted("Neural", "Debug", " - Connection RAM: " + DataSizeFormatter(sizeof(NodeConnection_GPU)));
	printFormatted("Neural", "Debug", " - Genomes RAM: " + DataSizeFormatter(gRAM));
	printFormatted("Neural", "Debug", " - Nodes RAM: " + DataSizeFormatter(nRAM));
	printFormatted("Neural", "Debug", " - Connections RAM: " + DataSizeFormatter(cRAM));
	printFormatted("Neural", "Debug", " - Total Memory Usage: " + DataSizeFormatter(gRAM + nRAM + cRAM));
	
	delete NGPU;
	NGPU = new Network_GPU();
	int totalNodeCountPerGenome = (inputNodes + nodesPerLayer*hiddenLayerCount + outputNodes);
	
	int inputConnections = inputNodes*nodesPerLayer;
	int hiddenConnections = nodesPerLayer*nodesPerLayer*(hiddenLayerCount - 1);
	int outputConnections = outputNodes*nodesPerLayer;
	int totalNodeConnectionsCountPerGenome = (inputConnections + hiddenConnections + outputConnections);
	
	delete NetworkOutputs;
	NetworkOutputs = new std::vector<std::vector<float>>(genomeCount);
	for (int g = 0; g < genomeCount; g++) {
		std::vector<float> tmp = std::vector<float>();
		NetworkOutputs->at(g) = tmp;
		NetworkOutputs->at(g).resize(outputNodes);
		for (int o = 0; o < outputNodes; o++) {
			NetworkOutputs->at(g).at(o) = 0;
		}
	}
	
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
	NGPU->connections.resize(totalNodeConnectionsCountPerGenome * genomeCount);
	
	int tempWeightsCount = 0;
	NGPU->nodes.clear();
	NGPU->nodes.resize(totalNodeCountPerGenome * genomeCount);
	
	size_t totalSize = 	sizeof(Genome_GPU) * NGPU->genomes.capacity() +
						sizeof(Node_GPU) * NGPU->nodes.capacity() +
						sizeof(NodeConnection_GPU) * NGPU->connections.capacity() +
						sizeof(Network_GPU);
	printFormatted("Neural", "Success", "Allocated " + DataSizeFormatter(totalSize) + " for Network: ");
	print();
	printFormatted("Neural", "Log", "Configuring New Neural Network");
	
	//WIP
	for (int i = 0; i < NGPU->nodes.size(); i++) {
		NGPU->nodes[i].ID = i;
		//NGPU->nodes[i].nTT = 1;	//Node trigger type. 0 for step, 1 for sigmoid
		NGPU->nodes[i].nTT = rand() % (numberOfAvailableActivationFunctions+1) - 1;
		
		NGPU->nodes[i].nB = getRandomFloat();	//Node Bias
		NGPU->nodes[i].pNB = NGPU->nodes[i].nB;
				
		int startOfGenome = (int)(totalNodeCountPerGenome * floor((double)NGPU->nodes[i].ID / (double)totalNodeCountPerGenome));
		NGPU->nodes[i].nII = i < startOfGenome + inputNodes;
		NGPU->nodes[i].nIO = i >= startOfGenome + inputNodes + nodesPerLayer*hiddenLayerCount;
				
		NGPU->nodes[i].nIV = 0;
		NGPU->nodes[i].pO = -99999;
		NGPU->nodes[i].wSI = tempWeightsCount;
		
		int startOfNonInputs = startOfGenome + inputNodes;
		int startOfLastLayer = startOfGenome;
		int currentLayer = std::floor(((i - startOfGenome) - inputNodes)/nodesPerLayer) + 1;
		
		if (i - startOfGenome >= inputNodes) {
			if (i - startOfGenome < totalNodeCountPerGenome - outputNodes) {
				// Node is hidden node
				if (currentLayer > 1) {
					startOfLastLayer = (currentLayer - 2)*nodesPerLayer + inputNodes + startOfGenome;
				}
			} else {
				// Node is output node
				startOfLastLayer = startOfGenome + totalNodeCountPerGenome - outputNodes - nodesPerLayer;
			}
		}
		
		// Everything before this point is confirmed working
		if (NGPU->nodes[i].nIO) {
			// Node is output node
			NGPU->nodes[i].wEI = tempWeightsCount + nodesPerLayer - 1;
			
			for (int x = 0; x < nodesPerLayer; x++) {
				int pos = tempWeightsCount;
				NGPU->connections[pos].NodePos = startOfLastLayer + x;
				NGPU->connections[pos].Weight = getRandomFloat();
				NGPU->connections[pos].Prev_Weight = NGPU->connections[pos].Weight;
				tempWeightsCount++;
			}
		} else if (!(NGPU->nodes[i].nII)) {
			// Node is hidden node
			bool isSecondLayer = i < startOfGenome + inputNodes + nodesPerLayer;
			
			NGPU->nodes[i].wEI = tempWeightsCount + nodesPerLayer - 1;
			if (isSecondLayer)
				NGPU->nodes[i].wEI = tempWeightsCount + inputNodes - 1;
			
			if (isSecondLayer) {
				for (int x = 0; x < inputNodes; x++) {
					int pos = tempWeightsCount;
					NGPU->connections[pos].NodePos = startOfGenome + x;
					NGPU->connections[pos].Weight = getRandomFloat();
					NGPU->connections[pos].Prev_Weight = NGPU->connections[pos].Weight;
					tempWeightsCount++;
				}
			} else {
				for (int x = 0; x < nodesPerLayer; x++) {
					int pos = tempWeightsCount;
					NGPU->connections[pos].NodePos = startOfGenome + inputNodes + x + nodesPerLayer*(currentLayer - 2);
					NGPU->connections[pos].Weight = getRandomFloat();
					NGPU->connections[pos].Prev_Weight = NGPU->connections[pos].Weight;
					tempWeightsCount++;
				}
			}
		}
	}
	
	// Now just check to make sure the network has no circular definitions and all nodes are ID'd
	int trimmableGenomes = 0;
	for (int i = 0; i < NGPU->genomes.size(); i++) {
		if (NGPU->genomes[i].ID == -1) { trimmableGenomes++; }
		if ((NGPU->genomes[i].Nodes_Start_Index > (int)NGPU->nodes.size() + 1) || (NGPU->genomes[i].Nodes_End_Index < 0)) {
			printFormatted("Neural","Error","While creating neural net. Genome indexing failure");
			quit();
		}
	}
			
	int trimmableNodes = 0;
	for (int i = 0; i < NGPU->nodes.size(); i++) {
		if (NGPU->nodes[i].ID == -1) { trimmableNodes++; }
		if ((NGPU->nodes[i].wEI > (int)NGPU->connections.size() + 1) || (NGPU->nodes[i].wSI < 0)) {
			printFormatted("Neural","Error","While creating neural net. Node indexing failure");
			quit();
		}
	}
	
	for (int i = 0; i < NGPU->connections.size(); i++) {
		if ((NGPU->connections[i].NodePos > NGPU->nodes.size()) || (NGPU->connections[i].NodePos < 0)) {
			printFormatted("Neural","Error","While creating neural net. Connection indexing failure");
			quit();
		}
	}
	
	if (trimmableGenomes > 0) { throw std::invalid_argument(std::to_string(trimmableGenomes) + " genomes found with unset ID"); }
	if (trimmableNodes > 0) { throw std::invalid_argument(std::to_string(trimmableNodes) + " nodes found with unset ID"); }
	
	_genomeCount =      genomeCount;
	_inputCount =       inputNodes;
	_nodesPerLayer =    nodesPerLayer;
	_hiddenLayerCount = hiddenLayerCount;
	_outputNodes =      outputNodes;
	
	printFormatted("Neural", "Success", "Neural Network generated");
}

void SaveNeuralNetworkNonBlocking(std::string dir, bool* status) {
	saveDirectory = dir + "/";
	SaveNeuralNetworkNonBlocking(status);
}
void SaveNeuralNetworkNonBlocking(bool* status) {
	std::thread t(SaveNeuralNetworkHandler, status);
	t.detach();
}

void SaveNeuralNetworkHandler(bool* status) {
	SaveNeuralNetwork();
	*status = false;
}

void SaveNeuralNetwork(std::string dir) { saveDirectory = dir + "/"; SaveNeuralNetwork(); }
void SaveNeuralNetwork() {
	print();
	printFormatted("Save", "Log", "Saving Neural Network");
	if (saveDirectory == "") {
		printFormatted("Save","Error","Could not save GPU network. SaveNeuralNetwork was called without setting save directory");
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
	printFormatted("Save", "Success", "Neural Network Saved");
}

void SaveNetworkGenomes_MTwTDC(ThreadDataContainer* TDC) {
	Genome_GPU G[TDC->EndIndex - TDC->ID + 1];
	
	for(int i = TDC->ID; i <= TDC->EndIndex; i++) {
		G[i - TDC->ID] = NGPU_Copy->genomes[i];
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
		N[i - TDC->ID] = NGPU_Copy->nodes[i];
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
		C[i - TDC->ID] = NGPU_Copy->connections[i];
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
	if (saveDirectory == "") {
		printFormatted("Save","Error","Could not save GPU network. SaveNeuralNetworkInternal was called without setting save directory");
		quit();
	}
	
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
	int maxThreadCount = std::thread::hardware_concurrency()/2;
	int currentThreadCount = 0;
			
	printFormatted("Save","Debug", "Genome File Count: " + std::to_string(ceil((float)NGPU_Copy->genomes.size()/(float)numberOfIndexesPerThread)));
	printFormatted("Save","Debug", "Node File Count: " + std::to_string(ceil((float)NGPU_Copy->nodes.size()/(float)numberOfIndexesPerThread)));
	printFormatted("Save","Debug", "Node Connections File Count: " + std::to_string(ceil((float)NGPU_Copy->connections.size()/(float)numberOfIndexesPerThread)));
	printFormatted("Save","Debug", "Index Count per CPU Thread: " + std::to_string(numberOfIndexesPerThread));	
	
	//Now we actually start saving the AI to disk
	bool genomesComplete = false;
	bool nodesComplete = false;
	while(run) {
		while(currentThreadCount < maxThreadCount) {
			if (genomeIndex >= NGPU_Copy->connections.size()) { run = false; break; }
			
			ThreadDataContainer* TDC_Connection = new ThreadDataContainer();
			TDC_Connection->ID = genomeIndex;
			TDC_Connection->EndIndex = genomeIndex + std::min(numberOfIndexesPerThread - 1, (int)NGPU_Copy->connections.size() - genomeIndex - 1);
			
			if (TDC_Connection->EndIndex >= NGPU_Copy->connections.size()) {
				printFormatted("Save","Error", "Connection indexing error!");
				quit();
			}
			
			TDC_Connection->path = dir;
			TDC_List.push_back(TDC_Connection);
			
			std::thread connectionThread(SaveNetworkConnections_MTwTDC, TDC_Connection);
			connectionThread.detach();
			
			currentThreadCount++;
			
			if (genomeIndex < NGPU_Copy->nodes.size()) {
				ThreadDataContainer* TDC_Node = new ThreadDataContainer();
				TDC_Node->ID = genomeIndex;
				TDC_Node->EndIndex = genomeIndex + std::min(numberOfIndexesPerThread - 1,(int)NGPU_Copy->nodes.size()-genomeIndex-1);
				TDC_Node->path = dir;
				
				if (TDC_Node->EndIndex >= NGPU_Copy->nodes.size()) {
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
			
			if (genomeIndex < NGPU_Copy->genomes.size()) {
				ThreadDataContainer* TDC_Genome = new ThreadDataContainer();
				TDC_Genome->ID = genomeIndex;
				TDC_Genome->EndIndex = genomeIndex + std::min(numberOfIndexesPerThread - 1,(int)(NGPU_Copy->genomes.size())-genomeIndex-1);
				TDC_Genome->path = dir;
				
				if (TDC_Genome->EndIndex >= NGPU_Copy->genomes.size()) {
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

void saveFileRepair() {
	print();
	if (saveDirectory == "") {
		printFormatted("File Repair","Error","Could not repair save files. saveFileRepair was called without setting save directory");
		quit();
	}
	
	std::string netSaveDir = saveDirectory + "Network Save Orig/";
	std::string netBackupDir = saveDirectory + "Network Save Backup/";
	std::string netStatusFile = saveDirectory + "saveStatus.txt";
	
	std::string intactDirectory = "";
	std::string corruptedDirectory = "";
	if (std::filesystem::exists(netStatusFile)) {
		//0 	Means it's completely safe to read from either.							Read from the first one
		//1 	Means it was in the middle of writing to the first copy. 				Read from the second
		//2 	Means it was in the middle of writing to the backup copy. 				Read from the first one
		//null  Means it was in the middle of writing to the save status directory.		Read from the first one
		std::string saveStatus;
		std::ifstream myfile;
		myfile.open(netStatusFile);
		std::getline(myfile,saveStatus);
		myfile.close();
		
		if (saveStatus == "0") {
			// Both saves should be fine
			printFormatted("File Repair","Log","No save file errors found");
		} else if (saveStatus == "1") {
			//Primary save is corrupted
			printFormatted("File Repair","Warning","Detected primary save folder corruption");
			if (std::filesystem::exists(netBackupDir)) {
				printFormatted("File Repair","Log","Detected intact backup save folder");
				intactDirectory = netBackupDir;
				corruptedDirectory = netSaveDir;
			} else {
				printFormatted("File Repair","Error","No backup save folder detected. Could not repair saved data");
				quit();
			}
		} else if (saveStatus == "2") {
			//Backup save is corrupted
			printFormatted("File Repair","Warning","Detected backup save folder corruption");
			if (std::filesystem::exists(netBackupDir)) {
				printFormatted("File Repair","Log","Detected intact primary save folder");
				intactDirectory = netSaveDir;
				corruptedDirectory = netBackupDir;
			} else {
				printFormatted("File Repair","Error","No primary save folder detected. Could not repair saved data");
				quit();
			}
		} else {
			printFormatted("File Repair","Warning","Invalid save status detected in save directory. Checking save directories");
			if ((!std::filesystem::exists(netBackupDir)) && (!std::filesystem::exists(netSaveDir))) {
				printFormatted("File Repair","Error","Missing both save directories!");
				quit();
			} else if (!std::filesystem::exists(netBackupDir)) {
				printFormatted("File Repair","Warning","Missing backup save directory");
			} else if (!std::filesystem::exists(netSaveDir)) {
			printFormatted("File Repair","Warning","Missing primary save directory");
			} else {
				printFormatted("File Repair","Log","Both save directories detected");
			}
			printFormatted("File Repair","Error","Due to unusual save directory status, cannot continue. Please manually check the save directory before restarting this program");
			quit();
		}
	} else {
		printFormatted("File Repair","Warning","Could not find save status file");
		if ((!std::filesystem::exists(netBackupDir)) && (!std::filesystem::exists(netSaveDir))) {
			printFormatted("File Repair","Error","Missing both save directories!");
			quit();
		} else if (!std::filesystem::exists(netBackupDir)) {
			printFormatted("File Repair","Warning","Missing backup save directory");
		} else if (!std::filesystem::exists(netSaveDir)) {
		printFormatted("File Repair","Warning","Missing primary save directory");
		} else {
			printFormatted("File Repair","Log","Both save directories detected");
		}
		printFormatted("File Repair","Error","Due to unusual save directory status, cannot continue. Please manually check the save directory before restarting this program");
		quit();
	}
	
	//intactDirectory
	//corruptedDirectory
	if (!std::filesystem::exists(intactDirectory)) {
		printFormatted("File Repair","Error","Could not find intact file directory for repairs: " + intactDirectory);
		quit();
	}
	
	if (std::filesystem::exists(corruptedDirectory))
		std::filesystem::remove_all(corruptedDirectory);
	
	std::filesystem::copy(intactDirectory,corruptedDirectory);
	printFormatted("File Repair","Success","Repaired save directory!");
}

void LoadNetworkGPU(std::string dir) { saveDirectory = dir + "/"; LoadNetworkGPU(); }
void LoadNetworkGPU() {
	print();
	printFormatted("Load","Log","Loading neural network from parent disk directory: " + saveDirectory);
	
	if (saveDirectory == "") {
		printFormatted("Load","Error","Could not load GPU network. LoadNetworkGPU was called without setting save directory");
		quit();
	}
	
	std::string netSaveDir    = saveDirectory + "Network Save Orig/";
	std::string netBackupDir  = saveDirectory + "Network Save Backup/";
	std::string netStatusFile = saveDirectory + "saveStatus.txt";
	
	std::string loadDirectory = "";
	if (std::filesystem::exists(netStatusFile)) {
		std::string saveStatus;
		std::ifstream myfile;
		myfile.open(netStatusFile);
		std::getline(myfile,saveStatus);
		myfile.close();
		
		if (saveStatus == "0") {
			printFormatted("Load","Log","Loading AI network from primary save");
			loadDirectory = netSaveDir;
		} else if (saveStatus == "1") {
			printFormatted("Load","Warning","Primary save corrupted. Attempting file repairs");
			saveFileRepair();
			printFormatted("Load","Log","Loading AI network from backup save");
			loadDirectory = netBackupDir;
		} else if (saveStatus == "2") {
			printFormatted("Load","Warning","Backup save corrupted. Attempting file repairs");
			saveFileRepair();
			printFormatted("Load","Log","Loading AI network from primary save");
			loadDirectory = netSaveDir;
		} else {
			printFormatted("Load","Warning","Invalid save status detected in save directory. Defaulting to primary save");
			loadDirectory = netSaveDir;
		}
	} else {
		printFormatted("Load","Warning","Could not find save status file. Searching for save directory directly");
		if (std::filesystem::exists(saveDirectory)) {
			if (std::filesystem::exists(netSaveDir)) {
				printFormatted("Load","Warning","Found direct copy of neural network primary save on disk. Uncertain save status detected, but continuing to load anyways");
				printFormatted("Load","Warning","Please note, since the save status file is missing, I cannot assure the purity of the saved network. Proceed with caution");
				loadDirectory = netSaveDir;
			} else if (std::filesystem::exists(netBackupDir)) {
				printFormatted("Load","Warning","Found direct copy of neural network backup save on disk. Uncertain save status detected, but continuing to load anyways");
				printFormatted("Load","Warning","Please note, since the save status file is missing, I cannot assure the purity of the saved network. Proceed with caution");
				loadDirectory = netSaveDir;
			} else {
				printFormatted("Load","Error","Could not load network. No save folder detected in directory:" + saveDirectory);
				quit();
			}
		} else {
			printFormatted("Load","Error","Could not load network. Save directory does not exist:" + saveDirectory);
			quit();
		}
	}
	
	delete(NGPU);
	
	auto start = std::chrono::high_resolution_clock::now();
	
	NGPU = new Network_GPU();
	
	//W.I.P
	// Now that we have confirmed the status of the save files and attempted any necessary repairs on them, we can now load up the neural network from disk.
	printFormatted("Load","Log","Loading from internal directory: " + loadDirectory);
	
	std::string connectionsTestData;
	std::ifstream connectionsTestFile;
	connectionsTestFile.open(loadDirectory + "Node Connections/Connection Data 0.csv");
	numberOfIndexesPerThread = 0;
	while(std::getline(connectionsTestFile,connectionsTestData)) { numberOfIndexesPerThread++; }
	connectionsTestFile.close();
	
	printFormatted("Load","Debug","Detected an IndexPerThread value of: " + std::to_string(numberOfIndexesPerThread));
	
	// Now we start pulling the required directories
	std::vector<std::string> genomes;
	std::vector<std::string> nodes;
	std::vector<std::string> connections;
	std::filesystem::path genomesDirectory{loadDirectory + "Genomes/"};
	std::filesystem::path nodesDirectory{loadDirectory + "Nodes/"};
	std::filesystem::path connectionsDirectory{loadDirectory + "Node Connections/"};
	for (auto const& dir_entry : std::filesystem::directory_iterator{genomesDirectory}) {
		std::string strFromPath = dir_entry.path().string();
		genomes.push_back(strFromPath);
	}
	for (auto const& dir_entry : std::filesystem::directory_iterator{nodesDirectory}) {
		std::string strFromPath = dir_entry.path().string();
		nodes.push_back(strFromPath);
	}
	for (auto const& dir_entry : std::filesystem::directory_iterator{connectionsDirectory}) {
		std::string strFromPath = dir_entry.path().string();
		connections.push_back(strFromPath);
	}
	printFormatted("Load","Debug","Detected " + std::to_string(genomes.size()) + " genomes");
	printFormatted("Load","Debug","Detected " + std::to_string(nodes.size()) + " nodes");
	printFormatted("Load","Debug","Detected " + std::to_string(connections.size()) + " connections");
	
	NGPU->genomes.resize(genomes.size() * numberOfIndexesPerThread);
	NGPU->nodes.resize(nodes.size() * numberOfIndexesPerThread);
	NGPU->connections.resize(connections.size() * numberOfIndexesPerThread);
	
	//for (std::string i : nodes) {
	//	print(i);
	//}
	
	// Now that we have all the directories, we can start calling the multithreads to load the network
	std::vector<ThreadDataContainer*> TDC_List;
	bool run = true;
	int genomeIndex = 0;
	int maxThreadCount = std::thread::hardware_concurrency()/2;
	int currentThreadCount = 0;
	while(run) {
		while(currentThreadCount < maxThreadCount) {
			if (genomeIndex == connections.size()) { run = false; break; }
			
			// Here we start the threads
			ThreadDataContainer* Connection_TDC = new ThreadDataContainer();
			Connection_TDC->ID = genomeIndex;
			Connection_TDC->path = connections[genomeIndex];
			TDC_List.push_back(Connection_TDC);
			std::thread connectionThread(LoadNetworkConnections_MTwTDC, Connection_TDC);
			connectionThread.detach();
			
			currentThreadCount++;
			
			if (genomeIndex < nodes.size()) {
				ThreadDataContainer* Node_TDC = new ThreadDataContainer();
				Node_TDC->ID = genomeIndex;
				Node_TDC->path = nodes[genomeIndex];
				TDC_List.push_back(Node_TDC);
				std::thread nodeThread(LoadNetworkNodes_MTwTDC, Node_TDC);
				nodeThread.detach();
				
				currentThreadCount++;
			}
			
			if (genomeIndex < genomes.size()) {
				ThreadDataContainer* Genome_TDC = new ThreadDataContainer();
				Genome_TDC->ID = genomeIndex;
				Genome_TDC->path = genomes[genomeIndex];
				TDC_List.push_back(Genome_TDC);
				std::thread genomeThread(LoadNetworkGenomes_MTwTDC, Genome_TDC);
				genomeThread.detach();
				
				currentThreadCount++;
			}
			
			genomeIndex++;
		}
		if (run) {
			for (int x = 0; x < TDC_List.size(); x++) {
				if (TDC_List[x]->threadCompletionStatus) {
					TDC_List.erase(TDC_List.begin() + x);
					currentThreadCount--;
					x--;
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
	
	printFormatted("Load","Log","Trimming network...");
	
	// Trim network of undeclared objects
	int i = NGPU->genomes.size();
	while(true) {
		i--;
		if (NGPU->genomes[i].ID == -1) {
			NGPU->genomes.erase(NGPU->genomes.begin() + i);
		} else { break; }
	}
	
	i = NGPU->nodes.size();
	while(true) {
		i--;
		if (NGPU->nodes[i].ID == -1) {
			NGPU->nodes.erase(NGPU->nodes.begin() + i);
		} else { break; }
	}
	
	i = NGPU->connections.size();
	while(true) {
		i--;
		if (NGPU->connections[i].NodePos == 0) {
			NGPU->connections.erase(NGPU->connections.begin() + i);
		} else { break; }
	}
	
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	printFormatted("Load","Debug","Real time taken to load network: " + TimeFormatter(duration.count()));
	printFormatted("Load","Success","Neural network loaded from disk");
}

void LoadNetworkGenomes_MTwTDC(ThreadDataContainer* TDC) {
	std::string saveStatus;
	std::ifstream myfile;
	myfile.open(TDC->path);
	
	std::string line;
	std::getline(myfile,line);
	while(line != "") {
		// Use "line" to get out the data
		std::vector<std::string> loadData = SplitString(line,',');
		int ID = std::stoi(loadData[0]);
		NGPU->genomes[ID].ID = 								ID;
		NGPU->genomes[ID].fitness = 					std::stof(loadData[1]);
		NGPU->genomes[ID].prev_fitness = 			std::stof(loadData[2]);
		NGPU->genomes[ID].Nodes_Start_Index = std::stoi(loadData[3]);
		NGPU->genomes[ID].Nodes_End_Index = 	std::stoi(loadData[4]);
		
		std::getline(myfile,line);
	}
	
	myfile.close();
	TDC->threadCompletionStatus = true;
}

void LoadNetworkNodes_MTwTDC(ThreadDataContainer* TDC) {
	std::string saveStatus;
	std::ifstream myfile;
	myfile.open(TDC->path);
	
	std::string line;
	std::getline(myfile,line);
	while(line != "") {
		// Use "line" to get out the data
		std::vector<std::string> loadData = SplitString(line,',');
		int ID = std::stoi(loadData[0]);
		NGPU->nodes[ID].ID = ID;
		NGPU->nodes[ID].nTT =	std::stoi(loadData[1]);
		NGPU->nodes[ID].nB =	std::stof(loadData[2]);
		NGPU->nodes[ID].nII = std::stoi(loadData[3]);
		NGPU->nodes[ID].nIO =	std::stoi(loadData[4]);
		NGPU->nodes[ID].nIV =	std::stof(loadData[5]);
		NGPU->nodes[ID].pO = 	std::stof(loadData[6]);
		NGPU->nodes[ID].wSI =	std::stoi(loadData[7]);
		NGPU->nodes[ID].wEI =	std::stoi(loadData[8]);
		
		std::getline(myfile,line);
	}
	
	myfile.close();
	TDC->threadCompletionStatus = true;
}

void LoadNetworkConnections_MTwTDC(ThreadDataContainer* TDC) {
	std::string saveStatus;
	std::ifstream myfile;
	myfile.open(TDC->path);
	
	int startVal = std::stoi(ASCII_To_Numeric(TDC->path));
	
	int i = 0;
	std::string line;
	std::getline(myfile,line);
	while(line != "") {
		// Use "line" to get out the data
		std::vector<std::string> loadData = SplitString(line,',');
		int ID = startVal + i;
		NGPU->connections[ID].NodePos = 		std::stoi(loadData[0]);
		NGPU->connections[ID].Weight =			std::stof(loadData[1]);
		NGPU->connections[ID].Prev_Weight =	std::stof(loadData[2]);
		
		std::getline(myfile,line);
		i++;
	}
	
	myfile.close();
	TDC->threadCompletionStatus = true;
}

bool SaveFileExists(std::string dir) {
	print();
	printFormatted("Check","Log","Loading neural network from parent disk directory: " + dir);
	
	if (dir == "") {
		printFormatted("Check","Error","No directory given. Cannot check save file status");
		quit();
	}
	
	std::string netSaveDir    = dir + "Network Save Orig/";
	std::string netBackupDir  = dir + "Network Save Backup/";
	std::string netStatusFile = dir + "saveStatus.txt";
	
	if (std::filesystem::is_directory(dir) &&
		std::filesystem::is_directory(netSaveDir) &&
		std::filesystem::is_directory(netBackupDir) &&
		std::filesystem::exists(netStatusFile)
	) {
		return true;
	}
	return false;
}

void SetNetworkFitnesses(std::vector<float> fitnesses) {
	if (fitnesses.size() != NGPU->genomes.size()) {
		printFormatted("Internal","Error", "Could not apply network fitness values as the number of values is not the same as the number of genomes");
		quit();
	}
	
	for (int i = 0; i < NGPU->genomes.size(); i++) {
		NGPU->genomes[i].fitness = fitnesses[i];
		//#error GPU implimentation of SetNetworkFitnesses incomplete
	}
}

std::vector<std::vector<float>> GetNetworkOutput(GLuint location) {
	if (!*savingInProgress) {
		*savingInProgress = true;
		
		NGPU_Copy = new Network_GPU();
		NGPU_Copy->genomes =     NGPU->genomes;
		NGPU_Copy->nodes =       NGPU->nodes;
		NGPU_Copy->connections = NGPU->connections;
		
		//Here we want to start up the multithreaded CPU saving process
		SaveNeuralNetworkNonBlocking(savingInProgress);
	}
	
	std::vector<float> rawGPUOutput;
	Get_SSBO_Buffer(rawGPUOutput,location);
	
	int outputsPerGenome = _outputNodes;
	std::vector<std::vector<float>> results(_genomeCount);
	for (int i = 0; i < _genomeCount; i++) {
		results[i] = std::vector<float>(outputsPerGenome);
		for (int z = 0; z < outputsPerGenome; z++) {
			results[i][z] = rawGPUOutput[z + i*outputsPerGenome];
		}
	}
	return results;
}

float GetRandomFloat(float min, float max) {
	if (!initiailizedRandomNumberGenerator) {
		srand((unsigned)time(NULL));
		initiailizedRandomNumberGenerator = true;
	}
	
	float random = ((float) rand()) / (float) RAND_MAX;
    float diff = max - min;
    float r = random * diff;
    return min + r;
}
