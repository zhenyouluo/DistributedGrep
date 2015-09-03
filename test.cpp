#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "grep.h"
//#include "grep_diy.h"

using namespace std;

//default command
string out_file = "output.txt";
string grep_cmd = "grep apple *.log";

int main( int argc, char **argv){
	
	if( argc != 1 ){
		out_file.assign(argv[1]);

		grep_cmd = argv[2];
		for(int i=3; i < argc; i++){
			grep_cmd += " ";
			grep_cmd += argv[i];
		}
	}

	grep_system( out_file, grep_cmd );
	
	//grep_diy( out_file, grep_cmd );

	return 0;
}