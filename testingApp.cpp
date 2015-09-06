#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <vector>
#include <stdlib.h>     // atoi
#include <unistd.h>     // sleep

#include "connections.h"
#include "grep.h"

using namespace std;

#define NODES_NUMBER 3

std::vector<std::string> address;

void generateLog(int machine)
{
    stringstream filename;
    filename << "machine." << machine << ".log";

    ifstream (filename.str());

    std::stringstream log;

    for (int i = 0; i < 20; ++i)
    {
        log << "Log Message A" << i << std::endl;
        log << "Log Message 1" << std::endl;
        log << "Log Message 2" << std::endl;
        log << "Log Message 3" << std::endl;
        log << "Log Message 4" << std::endl;
        log << "Log Message 5" << std::endl;
        log << "Log Message 6" << std::endl;
        log << "Log Message 7" << std::endl;
        log << "Log Message 8" << std::endl;
        log << "Log Message 9" << std::endl;
    }
}

void listeningThread(int serverPort)
{
    int ret;
    int connFd = open_socket(serverPort);

    int send = 1;
    ret = read(connFd, &send, sizeof(int));

    int replay = 1;
    ret = write(connFd, &replay, sizeof(int));
}

void connection_thread(std::string address,  int serverPort, int threadId)
{
    int connectionToServer;

    connect_to_server(address.c_str(), serverPort, &connectionToServer);

    int send = 1;
    int ret = write(connectionToServer, &send, sizeof(int) );

    send = 0;
    ret = read (connectionToServer, &send, sizeof(int));

    if (send != 1)
    {
        std::cout << "Error on " << address << std::endl;
    }
}

void listening(int serverPort)
{
    std::string input;

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
        threads.push_back(std::thread(connection_thread,address.at(i), serverPort, i));
    }

    for (auto& th : threads) th.join();

    std::cout << "All systems responded DONE" << std::endl;

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

    int serverPort = atoi(argv[1]);
    int machineId = atoi(argv[2]);
    int task;

    if (argc > 3)
    {
        task = atoi(argv[3]); // client or server
    }
    else
    {
        task = 0;
    }

    generateLog(machineId);

    if (task == false) // task == 0 -> server
    {
        listeningThread(serverPort);
        return 0;
    }
    else if (task == true) // task != o -> client
    {
        getAdress("Address", machineId);
        listening(serverPort);
        std::cout << "Exito!" << std::endl;
        // Call app
        //check result
    }

    return 0;

}