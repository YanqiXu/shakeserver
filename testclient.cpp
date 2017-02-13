#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "fifo.h"

using namespace std;
string receive_fifo = "Namereply";
string send_fifo = "Namerequest";

int main() {
  string word;
  string reply;

  while (1) {
	cout << "Enter the word you would like to search for:";
	cin >>  word;
	transform(word.begin(), word.end(), word.begin(), ::tolower);

	
	// create the FIFOs for communication
	
	Fifo sendfifo(send_fifo);
	Fifo recfifo(receive_fifo);
	
	cout << "Send:" << word << endl;
	sendfifo.openwrite();
	sendfifo.send(word);
	
	
	recfifo.openread();
	
	/* Get a message from a server */
	reply = recfifo.recv();
	while(reply.find("$END") == -1)
	{
	cout<< reply << endl;
	reply = recfifo.recv();
	}
	
	if (reply.find("$END") != -1)
	{
	
	cout<< reply.substr(0,reply.find("$END")) << endl;
	}
	
	

	recfifo.fifoclose();
	sendfifo.fifoclose();
  }


}
