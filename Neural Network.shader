
// TO IMPLIMENT - Include the activation functions as part of the learning process

// Using any version other than 430 causes OpenGL compile error due to used OpenGL version
#version 430

// Training Variable Constants
const float TRAINING_SPEED_TO_ACCURACY_RATIO = 0.001;
const float MAX_FAILED_ITERATIONS = 20;
const float GENOME_SURVIVAL_PERCENTAGE = 5;

// Activation Function Constants
const float HYPERPARAMETER_P_ReLU = 0.01;
const float HYPERPARAMETER_K_SOFTPLUS = 1;
const float HYPERPARAMETER_A_ELU = 1;

// Math Constants
const float PI = 3.14159265358979323846264338;
const float e = 2.71828182845904523536028747;

const float OUTPUT_ERROR_VALUE = -999999;

struct Genome_GPU {
	int ID;							// The genome index/ID
	float fitness;					// How good the genome has performed
	float prev_fitness;				// How good the genome previously performed
	int Nodes_Start_Index;			// The start of the genome in the nodes array
	int Nodes_End_Index;			// The end of the genome in the nodes array
	int FailedNetworkIterations;	// The number of times the genome has failed to improve
};
		
struct Node_GPU {
	int ID;		//Identification code		// The ID of this node
			
	int nTT;	//Node trigger typeof
	float nB;	//Node bias
	float pNB;	//Previous node bias
			
	bool nII; 	//node Is Input				// Is the node is an input or not
	bool nIO; 	//node Is Output			// Is the node is an output or not
	float nIV;	//node Input Value			// If the node is an input, what have we entered
	float pO;	//precalculated Output		// This variable just allows for quicker genome output computing
			
	int wSI;	//weights Start Index		// This is the position in the weights array where the start of this nodes connections are held
	int wEI;	//weights End Index			// This is the position in the weights array where the end of this nodes connections are held
};
		
struct NodeConnection_GPU {
	int NodePos;		// The position in the nodes array that this connection is referencing
	float Weight;		// The weight of this connection
	float Prev_Weight;	// The previous weight of this connection
};

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

//Static variables that are only written to the GPU once. gArr, nArr, and cArr will be read from on occassion by the CPU when getting an updated version of the neural network to save
layout(std430, binding = 0) buffer neuralNetwork0 { Genome_GPU gArr[];         };
layout(std430, binding = 1) buffer neuralNetwork1 { Node_GPU nArr[];           };
layout(std430, binding = 2) buffer neuralNetwork2 { NodeConnection_GPU cArr[]; };
layout(std430, binding = 8) buffer neuralNetwork8 { uint gArrSize; };

// Dynamic variables that will be shared between the GPU and CPU overtime
layout(std430, binding = 3) buffer neuralNetwork3 { float inputArray[];        };
layout(std430, binding = 4) buffer neuralNetwork4 { float outputArray[];       };
layout(std430, binding = 5) buffer neuralNetwork5 { int epoch;                 };  // Need to manually change on the CPU each iteration
layout(std430, binding = 6) buffer neuralNetwork6 { float genomeSurvivalPerc;  };

// Do we want the network to actually train, or just provide output values? If pretrained, set this to false
layout(std430, binding = 7) buffer neuralNetwork7 { bool doTraining;           };

// Generate a random number, given an input seed and the current epoch
// These values are predetermined from online, I have no idea how it works, I just know it does
// DO NOT CHANGE THESE VALUES OR REPETITION IS LIKELY, WILL BECOME NOT AS RANDOM
float random(int genomeID) {
    vec3 seed = vec3(float(genomeID), float(epoch), 42.0);
    return 2.0 * fract(sin(dot(seed, vec3(12.9898, 78.233, 37.719))) * 43758.5453) - 1.0;
}

void main() {
	// Get global ID
	uint ID = gl_GlobalInvocationID.x;
	
	int numOutputs = 0;
	int outputNodeIndex = 0;
	int nodesStart = gArr[ID].Nodes_Start_Index;
	int nodesEnd = gArr[ID].Nodes_End_Index;
	
	// Did the neural network perform better or worse than last iteration? Ignore if this is the first epoch
	bool revertToPrev = (epoch != 1) && (gArr[ID].fitness < gArr[ID].prev_fitness);
	if (doTraining) {
		if (revertToPrev) {
			gArr[ID].FailedNetworkIterations++;
			
			// Now we decide if it performed TOO badly. If so, reset the genome entirely.
			if (gArr[ID].FailedNetworkIterations > MAX_FAILED_ITERATIONS) {
				int betterGenomeCount  = 0;
				int bestGenomeIndex1st = 0;
				int bestGenomeIndex2nd = 0;
				int bestGenomeIndex3rd = 0;
				for (int g = 0; g < int(gArrSize); g++) {
					if (gArr[g].prev_fitness > gArr[ID].prev_fitness) { betterGenomeCount++; }
					
					if (gArr[g].prev_fitness > gArr[bestGenomeIndex3rd].prev_fitness) { bestGenomeIndex3rd = g; }
					if (gArr[g].prev_fitness > gArr[bestGenomeIndex2nd].prev_fitness) {
						bestGenomeIndex3rd = bestGenomeIndex2nd;
						bestGenomeIndex2nd = g;
					}
					if (gArr[g].prev_fitness > gArr[bestGenomeIndex1st].prev_fitness) {
						bestGenomeIndex2nd = bestGenomeIndex1st;
						bestGenomeIndex1st = g;
					}
				}
				
				if (betterGenomeCount/float(gArrSize) < genomeSurvivalPerc) {
					// We have performed too poorly and must be either reset entirely or genetically mutated with the best performing genomes
					bool randomize = random(int(ID)) > 0.0;
					for (int n = gArr[ID].Nodes_Start_Index; n <= gArr[ID].Nodes_End_Index; n++) {
						for (int c = nArr[n].wSI; c <= nArr[n].wEI; c++) {
							float selectedWeight = 0;
							float r = random(int(4.651*ID * n + 83.162*c/n));
							selectedWeight = r;
							
							if (!randomize) {
								int nIndex = n - gArr[ID].Nodes_Start_Index;
								int cIndex = c - nArr[n].wSI;
								if (r > 0.5) {
									nIndex += gArr[bestGenomeIndex1st].Nodes_Start_Index;
									selectedWeight = cArr[nArr[nIndex].wSI + cIndex].Prev_Weight;
								} else if (r > 0) {
									nIndex += gArr[bestGenomeIndex2nd].Nodes_Start_Index;
									selectedWeight = cArr[nArr[nIndex].wSI + cIndex].Prev_Weight;
								} else if (r > -0.5) {
									nIndex += gArr[bestGenomeIndex3rd].Nodes_Start_Index;
									selectedWeight = cArr[nArr[nIndex].wSI + cIndex].Prev_Weight;
								} else {
									selectedWeight = 4.651*ID*n + 83.162*c/n;
								}
							}
							
							cArr[c].Prev_Weight = selectedWeight;
							cArr[c].Weight = cArr[c].Prev_Weight;
						}
					}
					
					gArr[ID].fitness = 0;
					gArr[ID].prev_fitness = 0;
				}
			}
			
			gArr[ID].fitness = gArr[ID].prev_fitness;
		} else {
			gArr[ID].FailedNetworkIterations = 0;
		}
		
		gArr[ID].prev_fitness = gArr[ID].fitness;
	} else {
		revertToPrev = false;
	}
	
	// If required, revert back to a better working version of the genome
	float largestWeight = OUTPUT_ERROR_VALUE;
	for (int nI = nodesStart; nI <= nodesEnd; nI++) {
		for (int nC = nArr[nI].wSI; nC <= nArr[nI].wEI; nC++) {
			if (revertToPrev) { cArr[nC].Weight = cArr[nC].Prev_Weight; }
			
			cArr[nC].Weight += random(nC);
			
			if (abs(cArr[nC].Weight) > largestWeight) { largestWeight = abs(cArr[nC].Weight); }
		}
		
		if (revertToPrev) { nArr[nI].nB = nArr[nI].pNB; }
		
		nArr[nI].nB += random(nI);
	
		// I wish there was a way to store the result of this for the next iteration so that we don't recalculate every epoch.
		// Not a big deal as it takes such an insignificant amount of time, but still...
		if (nArr[nI].nIO) { numOutputs++; }
	}
	
	// Normalize the weights
	if (largestWeight != 0) {
		for (int nI = nodesStart; nI <= nodesEnd; nI++) {
			for (int nC = nArr[nI].wSI; nC <= nArr[nI].wEI; nC++) {
				cArr[nC].Weight = cArr[nC].Weight / largestWeight;
			}
		}
	}
	
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------------------------
	
	
	// Now that we have reverted and analyzed all of the network, we can finally get the networks new output
	// Here we want to get the neural networks output given it's inputs by computing all of the nodes from start to finish
	// For each node we compute it's output. Due to the way the CPU gives us the nodes in structured order, we know that we can just iterate directly through without needing to precompute a previous node
	// For more complex neural geometries that aren't in the standard node-layers structure, I may need to rethink this approach later. Not important for now though
	for (int nodeIndex = nodesStart; nodeIndex <= nodesEnd; nodeIndex++) {
		if (nArr[nodeIndex].nII) {
			nArr[nodeIndex].pO = inputArray[nodeIndex - nodesStart];
		} else {
			for (int connectionIndex = nArr[nodeIndex].wSI; connectionIndex <= nArr[nodeIndex].wEI; connectionIndex++) {
				nArr[nodeIndex].pO += cArr[connectionIndex].Weight * nArr[cArr[connectionIndex].NodePos].pO;
			}
			
			nArr[nodeIndex].pO += nArr[nodeIndex].nB;
			// Now that we have added up the weighted inputs for this node, we run it's activation function
			nArr[nodeIndex].pO = 0;
			switch (nArr[nodeIndex].nTT) {
			case -1: //Disable Node. This disables the node entirely, and is for decreasing memory requirements later
				break;
			case 0:  //Step
				nArr[nodeIndex].pO = nArr[nodeIndex].pO >= 0.0 ? 1 : 0;
				break;
			case 1:  //Sine
				nArr[nodeIndex].pO = sin(nArr[nodeIndex].pO);
				break;
			case 2:  //Modded Sine
				nArr[nodeIndex].pO = nArr[nodeIndex].pO * sin(nArr[nodeIndex].pO);
				break;
			case 3:  //Cosine
				nArr[nodeIndex].pO = cos(nArr[nodeIndex].pO);
				break;
			case 4:  //Modded Cosine
				nArr[nodeIndex].pO = nArr[nodeIndex].pO * cos(nArr[nodeIndex].pO);
				break;
			case 5:  //Sigmoid
				nArr[nodeIndex].pO = 1/(1 + pow(e, -nArr[nodeIndex].pO));
				break;
			case 6:  //SiLU
				nArr[nodeIndex].pO = nArr[nodeIndex].pO * 1/(1 + pow(e, -nArr[nodeIndex].pO));
				break;
			case 7:  //ReLU
				nArr[nodeIndex].pO = nArr[nodeIndex].pO<=0?0:nArr[nodeIndex].pO;
				break;
			case 8:  //Leaky ReLU
				nArr[nodeIndex].pO = nArr[nodeIndex].pO<=0?0.01*nArr[nodeIndex].pO:nArr[nodeIndex].pO;
				break;
			case 9:  //Parametric ReLU
				nArr[nodeIndex].pO = nArr[nodeIndex].pO<=0?HYPERPARAMETER_P_ReLU*nArr[nodeIndex].pO:nArr[nodeIndex].pO;
				break;
			case 10: //Softplus
				nArr[nodeIndex].pO = log(1 + pow(e,nArr[nodeIndex].pO));
				break;
			case 11: //Modified Softplus
				nArr[nodeIndex].pO = nArr[nodeIndex].pO * log(1 + pow(e,nArr[nodeIndex].pO));
				break;
			case 12: //Hyperparameterized Softplus
				nArr[nodeIndex].pO = log(1 + pow(e,HYPERPARAMETER_K_SOFTPLUS*nArr[nodeIndex].pO))/HYPERPARAMETER_K_SOFTPLUS;
				break;
			case 13: //SReLU
				nArr[nodeIndex].pO = max(-HYPERPARAMETER_A_ELU, nArr[nodeIndex].pO);
				break;
			case 14: //ELU
				nArr[nodeIndex].pO = nArr[nodeIndex].pO<=0?HYPERPARAMETER_A_ELU*(pow(e,nArr[nodeIndex].pO) - 1):nArr[nodeIndex].pO;
				break;
			case 15: //Mish
				nArr[nodeIndex].pO = nArr[nodeIndex].pO*tanh(log(1 + pow(e,nArr[nodeIndex].pO)));
				break;
			case 16: //Metallic Means
				nArr[nodeIndex].pO = (nArr[nodeIndex].pO + sqrt(pow(nArr[nodeIndex].pO,2) + 4))/2;
				break;
			default: //Default Node Response
				nArr[nodeIndex].pO = nArr[nodeIndex].pO;
				break;
			}
		
			// Finally, if we are the output node, we set the output array to our value
			if (nArr[nodeIndex].nIO) {
				outputArray[numOutputs*ID + outputNodeIndex] = nArr[nodeIndex].pO;
				outputNodeIndex++;
			}
		}
	}
}
