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
	
	printFormatted("Main", "Log", "Generating Neural Network");
	CreateNewLayeredNetwork(100,300,200,4,2);
	//CreateNewLayeredNetwork(3,3,2,4,2);
	printFormatted("Main", "Success", "Neural Network Generated");
	print();
	
	printFormatted("Main", "Log", "Saving Neural Network");
	//SaveNeuralNetwork("/home/andrey/Desktop/AI Network");
	//LoadNetworkGPU("/home/andrey/Desktop/AI Network");
	
	print();
	print("Hit \"Enter\" to close window");
	quit();
	
	
	
	
	
	
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
	
	//printFormatted("Main","Log","Received data from client: " + GetClientResponse());
	//SendToClient("Hello Python, from c++");
	//printFormatted("Main","Log","Received data from client: " + GetClientResponse());
	//SendToClient("Hopefully you receive this");
	
	// Set up the neural network with the parameters requested by AI_Core.cpp
	
	
	// Finally shut down the server	
	CloseClientSocket();
	printFormatted("MAIN","Success","Closed the client socket");
	print("Press any key to quit");
	endProgram();
}

