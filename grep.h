#ifndef GREP_H
#define GREP_H

#include <iostream>
#include <fstream>
#include <string>

#include <iomanip>
#include <cstdlib>

using namespace std;

int grep_system( string output, string cmd, bool *grepFinished);

int dummygrep(std::string filename, std::string grep, char* buffer);


#endif