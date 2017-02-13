/***************************************************************************
* nameservercpp  -  Program to serve of last name statistics
 *
* copyright : (C) 2009 by Jim Skon
*
* This program runs as a background server to a CGI program, providinging US Census
* Data on the frequency of names in response to requestes.  The rational for this 
* is to demonstrait how a background process can serve up information to a web program,
* saving the web program from the work of intiallizing large data structures for every
* call.
* 
* 
*
***************************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>
#include "fifo.h"
#include <utility>
#include <algorithm>
// Include stemming stuff
#include "english_stem.h"
#include <string>
#include <vector>

using namespace std;



vector<int> indexSearch(string word, map<string, vector<int> > refs);

int titleNum(vector<int> titles, int num);
//precondition: vector of integers of the titles mapped to
// and the position of a line.
//This function return the position of the first word of the title line and
//it is the title of the line that num indicates.

string getBold(string line, string word);
//precondition: give a line with a word needs to be bold.
//output a bold line.

string getLine(string word, string savedWord, ifstream & infile, int lineNum);
//Given the number of a line, output a line with saveWord bold.

/* Fifo names */
	string receive_fifo = "Namerequest";
	string send_fifo = "Namereply";

/* Server main line,create name MAP, wait for and serve requests */
int main() {

  	
  	map<string, vector<int> > refs; // The map of words/references
    string word,line,line1,line2,targetWord;
    string inMessage, outMessage;
    ifstream infile; // open the file
    int position=0;
    vector<int> results,titles;
    vector<int>::iterator myIterator;
    

    stemming::english_stem<char, std::char_traits<char> > StemEnglish;

    infile.open("/home/class/SoftDev/Shakespeare/Shakespeare2.txt");
    
    if (infile.fail())
	{
		cout << "Error Loading Database!"<<endl;
		//break;
	}
	else
	{
		cout << "Shakespeare data loaded!"<<endl;
	}
    
    while (!infile.fail())
    {
        getline(infile,line); // get the next line of code

        if (line.substr(0,5)=="BOOK:")// titles is vector of all the position of all titles
        {
            titles.push_back(position);
        }

        istringstream lineStream(line); // Create a string stream of the line

        while (lineStream >> word)
        { 
            //Convert word to lower case
    		transform(word.begin(), word.end(), word.begin(), ::tolower);
    		//Reduce to Stem
    		StemEnglish(word);
            refs[word].push_back(position); // push the word and the line position on the vector for this word
        }
        position = infile.tellg(); // get the position of the next line
    }
    
    
  // create the FIFOs for communication
  	Fifo recfifo(receive_fifo);
  	Fifo sendfifo(send_fifo); 
  
  
  if (infile.fail())
	{
	cout << "\Error!";
	infile.clear( );
	}
	
  while (1) {

    /* Get a message from a client */
    recfifo.openread();
    inMessage = recfifo.recv();
    
    
	infile.close();
    infile.open("/home/class/SoftDev/Shakespeare/Shakespeare2.txt");


    cout << "Enter the word you would like to search for:"+inMessage;

    string saveWord=inMessage;
    //Convert word to lower case
    transform(inMessage.begin(), inMessage.end(), inMessage.begin(), ::tolower);
    //Reduce to Stem
    StemEnglish(inMessage);

    results=indexSearch(inMessage,refs);
	sendfifo.openwrite();
	
    if (results.size()==0)
    {
        outMessage="<p>No Matches!</p>";
        sendfifo.send(outMessage);
        cout << outMessage << endl;
    }
    else
    {	
    	
        outMessage="<p>Here are all the lines containing the word "+saveWord+ ":</p>";
        sendfifo.send(outMessage);
        cout << outMessage << endl;
        
        vector<int>::iterator myIterator, myIterator2;
   		myIterator2=results.begin();
    	myIterator = results.begin();
        
        //print the first book and the first line in this book containing the searched word.
    	outMessage = "<h3>"+getLine(inMessage, saveWord,infile, titleNum(titles,*myIterator2))+"</h3>";
    	sendfifo.send(outMessage);
    	cout << outMessage << endl;
    	
    	outMessage = "<p style=\"font-family:courier;\" >"+getLine(inMessage,saveWord, infile, *myIterator2)+"</p>";
    	sendfifo.send(outMessage);
    	cout << outMessage << endl;
    	
		myIterator2++;
		
		//print the rest of the text
		
		while(myIterator2!=results.end())
		{
		
    		if( titleNum(titles,*myIterator)!=titleNum(titles,*myIterator2))
    		{
    			outMessage="<h3>"+getLine(inMessage,saveWord,infile, titleNum(titles,*myIterator2))+"</h3>";
    			sendfifo.send(outMessage);
    			cout << outMessage << endl;
    		}
    
    		outMessage="<p style=\"font-family:courier;\" >"+getLine(inMessage,saveWord, infile, *myIterator2)+"</p>";
    		sendfifo.send(outMessage);
    		cout << outMessage << endl;
    		
    		myIterator++;
    		myIterator2++;
    	}  
    }

	
	sendfifo.send("$END");
	cout << "$END" << endl;

	
	sendfifo.fifoclose();
	recfifo.fifoclose();

  }
}

vector<int> indexSearch(string word, map<string, vector<int> > refs)
{
     map<string, vector<int> >::iterator it;  // iterator for find
     vector<int> blank; // return for no matches
    /* find the word and get the vector of references */
    /* First use find, so as to NOT create a new entry */
    it = refs.find(word);
    if (it == refs.end())
    {
         return(blank);
    }
    else
    {
        return (refs[word]);
    }
}

int titleNum(vector<int> titles, int num)
{
    int i=0;
    while(titles[i]<num)
    {
         i++;
    }
    return(titles[i]);
}


string getLine(string word,string savedWord, ifstream & infile, int lineNum)
{
	string line;
	infile.seekg(lineNum,infile.beg);
	getline(infile,line);
	return (getBold(line,savedWord));
}

string getBold(string line, string word)
{
	string message;
	int position,end;
    position=line.find(word);
    if (position == -1)
    {
    message=line;
    }
    else
    {
    message += line.substr(0,position);
    message += "<b>"+ word + "</b>" + 
    line.substr(position+word.size());
    }
    
    return (message);   
}