#include <sstream>
#include <stdio.h>
#include <string.h>
#include "grep.h"

int grep_system( string output, string cmd ){
	
	//delete the output file
	string remove = "rm ";
	system( (remove + output + " 2>&-").c_str() );	/*hide error*/

	system( (cmd + ">>" + output).c_str() );

	return 0;
}

int dummygrep(std::string filename, std::string grep, char* buffer)
{
	std::stringstream ss;

	for (int i = 0; i < 1; ++i)
	{
		ss << "Log Message " << i << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
		ss << "Log Message kfjdsañlkfjadsklñhj3khnglñjkajgkhasljkgdsjkhewjuabhdshfgadsñjklfgadfhadsgfhjkads" << std::endl;
	}

	const std::string tmp = ss.str();
	const char* cstr = tmp.c_str();

       strcpy(buffer, cstr);

	return strlen(cstr);
}
