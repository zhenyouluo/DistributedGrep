#ifndef GREP_DIY_H
#define GREP_DIY_H


#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <regex.h>
#include <sys/types.h>

using namespace std;

int grep_diy( string output, string cmd );

#endif