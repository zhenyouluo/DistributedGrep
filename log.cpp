#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <stdlib.h>     // atoi
#include <unistd.h>     // , sleep

#include "connections.h"
#include "grep.h"

using namespace std;

int SERVER_PORT;

void listeningThread()
{
    int ret;
    int connFd = open_socket(SERVER_PORT);

    char buffer[256];
    ret = read (connFd, buffer, 256);

    std::cout << "Received: " << buffer << std::endl;

    std::string grep(buffer);

    unsigned int size = dummygrep("test.txt", grep, buffer);

    //std::string replay = "This is the reply from the server with the query";
    //std::strcpy(buffer, replay.c_str());
    ret = write(connFd, buffer, strlen(buffer));
}

void listeningCin()
{
    std::string input;
    int connectionToServer;

    getline(std::cin, input);
    std::cout << "You entered: " << input << std::endl;

    if (input.compare("exit") == 0)
    {
        std::cout << "Exiting normally " << std::endl;
        exit(0);
    }

    std::string address = "localhost";

    connect_to_server(address.c_str(), SERVER_PORT, &connectionToServer);

    int ret;
    char ack;
    char init = 'i';

    char * cstr = new char [input.length()+1];
    std::strcpy (cstr, input.c_str());
    //Write init
    ret = write(connectionToServer, cstr, input.length()+1 );

    char buffer[256];
    ret = read (connectionToServer, buffer, 256);

    std::cout << "Replay: " << buffer << std::endl;
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