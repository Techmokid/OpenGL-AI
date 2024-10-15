#version 430

struct Genome_GPU {
	int ID;							// The genome index/ID
	double fitness;					// How good the genome has performed
	double prev_fitness;			// How good the genome previously performed
	int Nodes_Start_Index;			// The start of the genome in the nodes array
	int Nodes_End_Index;			// The end of the genome in the nodes array
	int FailedNetworkIterations;	// The number of times the genome has failed to improve
};
		
struct Node_GPU {
	int ID;		//Identification code		// The ID of this node
			
	int nTT;	//Node trigger typeof
	double nB;	//Node bias
	double pNB;	//Previous node bias
			
	bool nII; 	//node Is Input				// Is the node is an input or not
	bool nIO; 	//node Is Output			// Is the node is an output or not
	double nIV;	//node Input Value			// If the node is an input, what have we entered
	double pO;	//precalculated Output		// This variable just allows for quicker genome output computing
			
	int wSI;	//weights Start Index		// This is the position in the weights array where the start of this nodes connections are held
	int wEI;	//weights End Index			// This is the position in the weights array where the end of this nodes connections are held
};
		
struct NodeConnection_GPU {
	int NodePos;		// The position in the nodes array that this connection is referencing
	double Weight;		// The weight of this connection
	double Prev_Weight;	// The previous weight of this connection
};

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer neuralNetwork {
  Genome_GPU gArr[];
  Node_GPU nArr[];
  NodeConnection_GPU cArr[];
};

void main() {
  uint ID = gl_GlobalInvocationID.x;
  Genome genome = gArr[ID];
  
  
  
  
  
  
  
  
}
