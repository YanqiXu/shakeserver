#include <iostream>
// Stuff for AJAX
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

//Stuff for pipes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include "fifo.h"

using namespace std;
using namespace cgicc; // Needed for AJAX functions.


//Indeed, myString needs to be a copy of the original string
std::string StringToLower(std::string myString)
{
  const int length = myString.length();
  for(int i=0; i!=length ; ++i)
    {
      myString[i] = std::tolower(myString[i]);
    }
  return myString;
}


// fifo for communication
string receive_fifo = "Namereply";
string send_fifo = "Namerequest";

int main() {
  Cgicc cgi;    // Ajax object
  // Create AJAX objects to recieve information from web page.
  
  form_iterator word = cgi.getElement("word");
  // Call server to get results
  string stword = **word;	
  
  cout << "Content-Type: text/plain\n\n";
  //cout <<"The word is" << stword << endl;
 

  // create the FIFOs for communication
  
  Fifo sendfifo(send_fifo);
  Fifo recfifo(receive_fifo);
  
  //stword = StringToLower(stword);
  string message =  stword;
  sendfifo.openwrite();
  sendfifo.send(message);
  
  /* Get a message from a server */

  	string reply;
  	recfifo.openread();
	
	reply = recfifo.recv();
	
	while(reply.find("$END") == -1)
	{
		cout << reply << endl;
		reply = recfifo.recv();
	}
	
	if (reply.find("$END") != -1)
	{
		//reply= recfifo.recv();
		cout << reply.substr(0,reply.find("$END")) << endl;
	}
	
	
	recfifo.fifoclose();
	sendfifo.fifoclose();
	
	return 0;
}
