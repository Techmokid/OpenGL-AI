#include "functions.h"

#define TEMP_SAVE_LOCATION expandEnvironmentVariables("C:/Temp")

int MatchingNetworks(Network_GPU* a, Network_GPU* b) {
    if (a->genomes.size()         != b->genomes.size())         { return 0; }
    if (a->nodes.size()           != b->nodes.size())           { return 1; }
    if (a->connections.size() != b->connections.size()) { return 2; }
    
    for (int g = 0; g < a->genomes.size(); g++) {
        if (a->genomes[g].ID                        != b->genomes[g].ID)                        { return 3; }
        if (a->genomes[g].Nodes_Start_Index         != b->genomes[g].Nodes_Start_Index)         { return 6; }
        if (a->genomes[g].Nodes_End_Index           != b->genomes[g].Nodes_End_Index)           { return 7; }
        if (a->genomes[g].FailedNetworkIterations   != b->genomes[g].FailedNetworkIterations)   { return 8; }
        
        for (int n = 0; n < a->nodes.size(); n++) {
            if (a->nodes[n].ID  != b->nodes[n].ID)  { return 9; }
            if (a->nodes[n].nII != b->nodes[n].nII) { 
                // Add detailed logging to identify the mismatch
                std::cout << "Mismatch found in nII at node " << n << std::endl;
                std::cout << "Network A node " << n << " nII: " << a->nodes[n].nII << std::endl;
                std::cout << "Network B node " << n << " nII: " << b->nodes[n].nII << std::endl;
                return 13; 
            }
            if (a->nodes[n].nIO != b->nodes[n].nIO) { return 14; }
            if (a->nodes[n].nIV != b->nodes[n].nIV) { return 15; }
            if (a->nodes[n].wSI != b->nodes[n].wSI) { return 17; }
            if (a->nodes[n].wEI != b->nodes[n].wEI) { return 18; }
            
            for (int c = 0; c < a->connections.size(); c++) {
                if (a->connections[c].NodePos != b->connections[c].NodePos) { return 19; }
            }
        }
    }
    
    return -1;
}

void RemoveTempFiles() {
	if (std::filesystem::exists(TEMP_SAVE_LOCATION + "/AI Network TEST A"))
		std::filesystem::remove_all(TEMP_SAVE_LOCATION + "/AI Network TEST A");
	if (std::filesystem::exists(TEMP_SAVE_LOCATION + "/AI Network TEST B"))
		std::filesystem::remove_all(TEMP_SAVE_LOCATION + "/AI Network TEST B");
	if (std::filesystem::exists(TEMP_SAVE_LOCATION + "/AI Network TEST C"))
		std::filesystem::remove_all(TEMP_SAVE_LOCATION + "/AI Network TEST C");
}

void RunTests() {
	printFormatted("Main", "Log", "Checking for neural network architecture anomolies");
	SilenceConsoleOutput(true);
	RemoveTempFiles();
	
	print("Creating Network A");
	CreateNewLayeredNetwork(3, 3, 4, 5, 6);
	print("Saving Network A");
	SaveNeuralNetwork(TEMP_SAVE_LOCATION + "/AI Network TEST A");
	print("Completed Network A Primary Creation");
	
	print("Creating Network B");
	CreateNewLayeredNetwork(10, 5, 8, 5, 2);
	print("Saving Network B");
	SaveNeuralNetwork(TEMP_SAVE_LOCATION + "/AI Network TEST B");
	print("Completed Network B Primary Creation");
	
	print("Creating Network C");
	CreateNewLayeredNetwork(3, 3, 4, 5, 6);
	print("Saving Network C");
	Network_GPU* NGPU_Copy_C = new Network_GPU(GetNetworkPointer()->clone());
	print("Completed Network C Primary Creation");
	
	print("Loading Network A");
	LoadNetworkGPU(TEMP_SAVE_LOCATION + "/AI Network TEST A");
	Network_GPU* NGPU_Copy_A = new Network_GPU(GetNetworkPointer()->clone());
	print("Loading Network B");
	LoadNetworkGPU(TEMP_SAVE_LOCATION + "/AI Network TEST B");
	Network_GPU* NGPU_Copy_B = new Network_GPU(GetNetworkPointer()->clone());
	print("Loaded All Test Networks!");
	
	RemoveTempFiles();
	SilenceConsoleOutput(false);

	int AC = MatchingNetworks(NGPU_Copy_A, NGPU_Copy_C);
	int AB = MatchingNetworks(NGPU_Copy_A, NGPU_Copy_B);
	int BC = MatchingNetworks(NGPU_Copy_B, NGPU_Copy_C);
	
	if (!((AC == -1) && (AB == 0) && (BC == 0))) {
		printFormatted("NetTest","ERROR","Generate/Save/Load system failed at least one test");
		printFormatted("NetTest","ERROR","Expected results: -1, 0, 0");
		printFormatted("NetTest","ERROR","Test results: " + std::to_string(AC) + "," + std::to_string(AB) + "," + std::to_string(BC));
		printFormatted("NetTest","LOG","Intentionally matching networks: " + std::to_string(AC));
		printFormatted("NetTest","LOG","Completely different networks 1: " + std::to_string(AB));
		printFormatted("NetTest","LOG","Completely different networks 2: " + std::to_string(BC));
		quit();
	}
	
	delete NGPU_Copy_A;
	delete NGPU_Copy_B;
	delete NGPU_Copy_C;
	
	printFormatted("Main", "Success", "No network anomolies detected");
}