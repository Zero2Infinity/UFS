/*************************
Creator: Rahul A. Patel  *
Date: July 2014          *
**************************/

#include <iostream>
#include <vector>
#include <cstdlib>
#include "constants.h"
#include "filesystem.h"

extern int initialize_disk(int);

using namespace std;
// filesystem related objects
CFileDescriptor FD[32];
// FILESYSTEM API
int openFile(string, string);
int writeToFile(int, unsigned int, string);
// shell interface and supporting functions
void shell();
void getTokens(string, vector<string>&);
int parseCommand(string);
void _tolowercase(string&);
int convertModeFromStrtoInt(string mode);

// Other declarations
enum CMD { CLEAR = 1, OPEN = 2, WRITE = 3, READ = 4, LSEEK = 5, CLOSE = 6, CAT = 7, END = 8};

// Entry point
int main() {
	initialize_disk(1); // Use existing disk..!
	shell();
	cout << "Goodbye!" << newl;
	return 0;
}

void shell() {
	string strInput;
	vector<string> tokens;
	int cmd, status;
	bool loop = true;
	cout<<"*** Welcome to Unix-like file system! ***" << newl;
	while(loop) {
		cout << "SHELL> ";
		getline(cin, strInput);
		tokens.clear();	// remove all the elements from the vector
		getTokens(strInput, tokens);
		cmd = parseCommand(tokens[0]);
		switch (cmd) {
			case CLEAR:
						//initialize_disk(1);
						break;
			case OPEN: 	
						openFile(tokens[1], tokens[2]);
						break;
			case WRITE:
						writeToFile(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), tokens[3]);
						break;
			case READ:
						break;
			case LSEEK:
						break;
			case CLOSE:
						break;
			case CAT:
						break;
			case END:
						cout << "about to exit..." << newl;
						loop = false;
						break;
			default: 
				cout << tokens[0] << ": command not found." << newl;
		}
	} 
}

// should return FD# (0-31) in future
int openFile(string filename, string mode) {
	int index = -1;
	int flagMode = FLAG_NONE;
	
	// find empty FD spot in array
	for (int i=0; i<MAX_FD_SIZE; i++) {
		if (FD[i].getStatus() == EMPTY || FD[i].getStatus() == ENTRY_NOT_EXISTS) {
			index = i;
			break;
		}
	}
	
	if (index == -1) {
		cout << "Not allowed to open more than 32 file handlers" << newl;
		return -1;
	}

	flagMode = convertModeFromStrtoInt(mode);
	FD[index].setFileName(filename.c_str());
	FD[index].setFileMode(flagMode);

	// Let FileSystem take care of this.
	FD[index].openFileCmd(filename.c_str(), flagMode);
	
	cout << "FD index: " << index << newl;
	return 0; 
}

int writeToFile(int fdIndex, unsigned int nBytes, string text) {
	cout << "Inside writeToFile.." << newl;
	//cout << fdIndex << " - " << nBytes << " - " << text << newl;
	if (fdIndex < 0 || fdIndex > MAX_FD_SIZE) {
		cout << "Invalid File Descriptor. Please provide correct index." << newl;
		return -1;
	}
	if (FD[fdIndex].getFileMode() == FLAG_WRITE 
		|| FD[fdIndex].getFileMode() == FLAG_READWRITE 
		|| FD[fdIndex].getFileMode() == FLAG_APPEND) 
	{
			
			FD[fdIndex].writeToFileCmd(nBytes, text);
	}
	cout << "Exiting writeToFile.." << newl;
	return 0;
}

// Utility functions
void getTokens(string line, vector<string>& v) {
	size_t lpos = 0, rpos = 0;
	size_t firstHashSymbol = line.find(HASH);
	size_t lastWhiteSpace = line.rfind(WHITESPACE, firstHashSymbol);
	// cout << "firstHashSymbol" << firstHashSymbol << "lastWhiteSpace" << lastWhiteSpace << newl;
	if (lastWhiteSpace == string::npos) {
		v.push_back(line); // contains one word
	} else {
		while (rpos != lastWhiteSpace) {
			rpos = line.find(WHITESPACE, lpos);
			v.push_back(line.substr(lpos, rpos - lpos));
			lpos = rpos + 1;
		}
		//cout << "Debug: " << line.substr(lastWhiteSpace + 1, line.length() - lastWhiteSpace) << newl;
		v.push_back(line.substr(lastWhiteSpace + 1, line.length() - lastWhiteSpace));
	}
}

int parseCommand(string cmd) {
	_tolowercase(cmd);
	if (cmd == "clear")
		return CLEAR;
	else if (cmd == "open")
		return OPEN;
	else if (cmd == "write")
		return WRITE;
	else if (cmd == "read")
		return READ;
	else if (cmd == "lseek")
		return LSEEK;
	else if (cmd == "close")
		return CLOSE;
	else if (cmd == "cat")
		return CAT;
	else if (cmd == "end")
		return END;

	return 0; // success
}

void _tolowercase(string& input) {
	for (int i=0; i< input.length(); i++)
		input[i] = tolower(input[i]);
}

int convertModeFromStrtoInt(string mode) {
	if (mode == "r")
		return FLAG_READ;
	else if (mode == "w")
		return FLAG_WRITE;
	else if (mode == "a")
		return FLAG_APPEND;
	else if (mode == "rw")
		return FLAG_READWRITE;
	
	return FLAG_NONE;
}
