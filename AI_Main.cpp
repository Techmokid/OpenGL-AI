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
#define NEURAL_NETWORK_SAVE_LOCATION "/home/andrey/Desktop/AI Network"
//#define NEURAL_NETWORK_SAVE_LOCATION "%USERPROFILE%/Desktop/AI Network"

GLuint CSH_GetNetworkOutput = -1;
GLuint CSH_TrainingNetwork = -1;

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
	
	CreateNewLayeredNetwork(10000, 365, 150, 3, 2);
	SaveNeuralNetwork(NEURAL_NETWORK_SAVE_LOCATION);
	
	print("Press any key to quit");
	endProgram();
}
