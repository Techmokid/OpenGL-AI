#include "functions.h"

#define printFormattingModuleLength 10
#define PRINT_DEBUGGING_MSG

#ifdef __WIN32
#define black			0
#define d_blue			1
#define green			2
#define turqoise		3
#define d_red			4
#define d_purple		5
#define d_yellow		6
#define gray			7
#define d_gray			8
#define blue			9
#define l_green			10
#define cyan			11
#define red				12
#define p_purple		13
#define yellow			14
#define white			15
#else
#define black			"\033[30m"
#define red			"\033[31m"
#define green			"\033[32m"
#define yellow			"\033[33m"
#define blue			"\033[34m"
#define magenta 		"\033[35m"
#define cyan			"\033[36m"
#define white			"\033[37m"
#define gray			"\033[90m"
#endif

void ClearConsole() { std::system("reset"); }

void print() { std::cout << std::endl; }
void print(int x) { std::cout << x << std::endl; }
void print(float x) { std::cout << x << std::endl; }
void print(double x) { std::cout << x << std::endl; }
void print(long unsigned int x) { std::cout << x << std::endl; }
void print(std::string x) { std::cout << x << std::endl; }

void waitForUserInput () {
	std::system("read ans");
}

std::string stringToLower(std::string in) {
   std::string result = "";
   for (int i = 0; i < in.length(); i++) {
      result += std::tolower(in[i]);
   }
   return result;
}

std::string stringToUpper(std::string in) {
   std::string result = "";
   for (int i = 0; i < in.length(); i++) {
      result += std::toupper(in[i]);
   }
   return result;
}

std::string stringToLowerCapitalized(std::string in) {
   std::string result = stringToLower(in);
   result[0] = std::toupper(result[0]);
   return result;
}

#ifdef __WIN32
void SetConsoleColor(int colour) {
   HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
   SetConsoleTextAttribute(hCon,colour);
}
#else
void SetConsoleColor(std::string colour) { std::cout << colour; }
#endif

void printSuccess(std::string module, std::string msg) { printFormatted(module, "success", msg); }
void printError(std::string module,   std::string msg) { printFormatted(module, "error", msg); }
void printLog(std::string module,     std::string msg) { printFormatted(module, "log", msg); }
void printInfo(std::string module,    std::string msg) { printFormatted(module, "info", msg); }
void printDebug(std::string module,   std::string msg) { printFormatted(module, "debug", msg); }
void printFormatted(std::string module, std::string status, std::string msg) {
   #ifndef PRINT_DEBUGGING_MSG
	 if (stringToLowerCapitalized(status) == "Debug") { return; }
	 #endif
	 
   module = stringToLowerCapitalized(module);

   std::cout << "[" + module + "]";
   for (int i = 0; i <	printFormattingModuleLength - module.length(); i++) { std::cout << " "; }
	
   std::cout << "[";
   status = stringToLowerCapitalized(status);

   if (status.length() > 7) { status = "???????"; }
   if (status == "Success") {
      SetConsoleColor(green);
      std::cout << "Success";
   } else if (status == "Warning") {
      SetConsoleColor(yellow);
      std::cout << "Warning";
   } else if (status == "Error") {
      SetConsoleColor(red);
      std::cout << " Error ";
   } else if (status == "Log") {
      SetConsoleColor(gray);
      std::cout << "  Log  ";
   } else if (status == "Info") {
      SetConsoleColor(gray);
      std::cout << "  Info ";
   } else if (status == "Debug") {
      #ifdef __WIN32
      SetConsoleColor(d_purple);
      #else
      SetConsoleColor(magenta);
      #endif
      
      std::cout << " Debug ";
   } else {
      SetConsoleColor(cyan);
      std::cout << "???????";
   }
   
   SetConsoleColor(white);
   std::cout << "]: " + msg << std::endl;
}

std::string TimeFormatter(double x) {
	if (x >= 7 * 24 * 60 * 60 * 1000) {
		return std::to_string((int)std::floor(x/(7*1000*60*60*24))) + " weeks";
	} else if (x >= 24 * 60 * 60 * 1000) {
		return std::to_string((int)std::floor(x/(1000*60*60*24))) + " days";
	} else if (x >= 60 * 60 * 1000) {
		return std::to_string((int)std::floor(x/(1000*60*60))) + " hours";
	} else if (x >= 60 * 1000) {
		return std::to_string((int)std::floor(x/(1000*60))) + " minutes";
	} else if (x >= 1000) {
		return std::to_string((int)std::floor(x/1000)) + " seconds";
	} else if (x >= 1) {
		return std::to_string((int)std::floor(x)) + " milliseconds";
	} else if (x >= 1/1000) {
		return std::to_string((int)std::floor(x*1000)) + " microseconds";
	} else if (x >= 1/(1000*1000)) {
		return std::to_string((int)std::floor(x*1000*1000)) + " nanoseconds";
	} else if (x >= 1/(1000*1000*1000)) {
		return std::to_string((int)std::floor(x*1000*1000*1000)) + " picoseconds";
	} else if (x <= 1/(1000*1000*1000)) {
		return "Time too small for formatting";
	} else {
		return "Time too large for formatting";
	}
}

std::vector<std::string> SplitString(std::string inputVal, char delimiter) {
	std::vector<std::string> result;
	result.push_back("");
	int x = 0;
	for (int i = 0; i < inputVal.length(); i++) {
		if (inputVal[i] == delimiter) {
			result.push_back("");
			x++;
		} else {
			result[x] += inputVal[i];
		}
	}
	return result;
}

std::string ASCII_To_Numeric(std::string x) {
	std::string result = "";
	for (int i = 0; i < x.length(); i++) {
		if (x[i] >= '0' && x[i] <= '9')
			result += x[i];
	}
	return result;
}
