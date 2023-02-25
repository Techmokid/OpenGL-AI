#include <iostream>
#include <thread>

using namespace std;

struct Results {
	int input = 0;
	int output = 0;
	bool threadComplete = false;
};

int main() {
  Results* arr[1000];
  
  cout << "Starting threads..." << endl;
  for (int i = 0; i < 8; i++) {
		arr[i] = new Results();
		arr[i]->input = i;
		thread myThread(myThreadableFunction, arr[i]);
		myThread.detach();
	}
	
	cout << "Awaiting threads completion" << endl;
	bool allThreadsStopped = false;
	while (!allThreadsStopped) {
		allThreadsStopped = true;
		for (int i = 0; i < 8; i++) {
			allThreadsStopped = (allThreadsStopped && arr[i]->threadComplete);
		}
	}
	
	cout << "All threads finished! Listing results now: " << endl;
	for (int i = 0; i < 8; i++) {
		cout << arr[i]->output << endl;
	}
	
	return 1;
}

void myThreadableFunction(Results* r) {
	r->output = r->input * 3;
	r->threadComplete = true;
}
