#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <vector>
#include <stdlib.h>     // atoi
#include <unistd.h>     // sleep
#include <stdio.h>
#include <string.h>

#include "connections.h"
#include "grep.h"

using namespace std;

#define BUFFER_MAX 1024*1024*8
#define NODES_NUMBER 3

int SERVER_PORT;

std::vector<std::stringstream> logs(NODES_NUMBER);
std::vector<std::string> address;

void listeningThread(int serverPort)
{
    int ret;
    int connFd = open_socket(serverPort);

    char* buffer = new char[BUFFER_MAX];
    ret = read (connFd, buffer, BUFFER_MAX);

    //std::cout << "Received: " << buffer << std::endl;

    std::string grep(buffer);

    unsigned int size = dummygrep("test.txt", grep, buffer);

    //std::string replay = "This is the reply from the server with the query";
    //std::strcpy(buffer, replay.c_str());
    ret = write(connFd, buffer, strlen(buffer));
}

void connection_thread(std::string input, std::string address, int serverPort, int threadId)
{
    int connectionToServer;

    connect_to_server(address.c_str(), serverPort, &connectionToServer);

    int ret;
    char ack;
    char init = 'i';

    char * cstr = new char [input.length()+1];
    std::strcpy (cstr, input.c_str());
    //Write init
    ret = write(connectionToServer, cstr, input.length()+1 );

    char* buffer = new char[BUFFER_MAX];
    ret = read (connectionToServer, buffer, BUFFER_MAX);

    std::string tmp(buffer);

    logs.at(threadId) << tmp;

    //std::cout << "Reply: " << buffer << std::endl;
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

    std::vector <std::thread> threads;

    for (int i = 0; i < NODES_NUMBER-1; ++i)
    {
        threads.push_back(std::thread(connection_thread,input, address.at(i), SERVER_PORT, i));
    }

    for (auto& th : threads) th.join();

    for (int i = 0; i < logs.size(); ++i)
    {
        std::cout << "Machine " << i << std::endl;
        std::cout << logs.at(i).str() <<std::endl;
    }

}

void getAdress(std::string filename, int machineId)
{
    stringstream file;
    file << filename << machineId << ".add";

    ifstream addFile(file.str());

    for (int i = 0; i < NODES_NUMBER -1; ++i)
    {
        std::string str;
        getline(addFile, str);
        address.push_back(str);
        std::cout << "Address " << i << ": " << str << std::endl;
    }
}


int main (int argc, char* argv[])
{
    char a;
    bool flag;

    SERVER_PORT = atoi(argv[1]);

    std::cout << "Distributed Logging init." << std::endl;

    std::thread listeningServer(listeningThread, SERVER_PORT);
    
    usleep(1000);

    std::cout << "Type 'grep' if you want to see logs: ";
    std::thread cinListening(listeningCin);

    cinListening.join();
    listeningServer.join();

    return 0;
}