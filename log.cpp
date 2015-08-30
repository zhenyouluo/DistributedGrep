
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <stdlib.h>     // atoi
#include <unistd.h>		// , sleep

#include "connections.h"

using namespace std;

int SERVER_PORT;

void listeningThread()
{
	int connFd = open_socket(SERVER_PORT);

    char ack;
    read (connFd, &ack, sizeof(ack));

    std::cout << "Received: " << ack << std::endl;

    //return true;
}

void listeningCin()
{
	std::string input;
	int connectionToServer;

	getline(std::cin, input);
	std::cout << "You entered: " << input << std::endl;

	std::string address = "localhost";

	connect_to_server(address.c_str(), SERVER_PORT, &connectionToServer);

	int ret;
    char ack;
    char init = 'i';
    //Write init
    ret = write(connectionToServer, &init, sizeof(char) );

}

int main (int argc, char* argv[])
{
    char a;
    bool flag;

    SERVER_PORT = atoi(argv[1]);

    std::cout << "Distributed Logging init." << std::endl;
    std::thread serverListening(listeningThread);
    usleep(1000);


    std::cout << "Type 'grep' if you want to see logs: ";
    std::thread cinListening(listeningCin);


    cinListening.join();

    serverListening.join();


    return 0;
}