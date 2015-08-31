#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

int dummygrep(std::string filename, std::string grep, char* buffer)
{
	std::stringstream ss;

	for (int i = 0; i < 20; ++i)
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

	std::strcpy(buffer, cstr);

	return strlen(cstr);
}