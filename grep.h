#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

int dummygrep(std::string filename, std::string grep, char* buffer)
{
	std::stringstream ss;

	ss << "Log Message 1" << std::endl;
	ss << "Log Message 2" << std::endl;
	ss << "Log Message 3" << std::endl;
	ss << "Log Message 4" << std::endl;
	ss << "Log Message 5" << std::endl;
	ss << "Log Message 6" << std::endl;
	ss << "Log Message 7" << std::endl;
	ss << "Log Message 8" << std::endl;
	ss << "Log Message 9" << std::endl;

	const std::string tmp = ss.str();
	const char* cstr = tmp.c_str();

	std::strcpy(buffer, cstr);

	return strlen(cstr);
}