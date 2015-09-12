#include <unistd.h>
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
#include <sys/poll.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mutex>

#include "connections.h"
#include "grep.h"
#include "constant.h"
#include "message.h"

using namespace std;

std::mutex printLogLock;

int SERVER_PORT;

std::vector<std::stringstream> logs(NODES_NUMBER);
std::vector<std::string> address;

void printLog(char * result, int threadId){
    printf("\nLog from machine %d:\n%s\n", threadId, result);
    return;
}



//Server Thread
void listeningThread(int serverPort)
{   
    //set connection
    int ret;
    int connFd = open_socket(serverPort);
    
    //recv command from client
    char* buffer = new char[BUFFER_MAX];
    ret = read (connFd, buffer, BUFFER_MAX);

    //get temp output filename
    string filename;
    stringstream ss;
    srand(time(NULL));
    ss<<rand()%100000;
    ss>>filename;
    filename += ".tmp";
    //cout<<"Server: filename is "<<filename<<endl;

    //grepping
    bool grepFinished = false;
    //std::thread systemCmd(grep_system, filename, "grep apple *.log", &grepFinished);
    std::thread systemCmd(grep_system, filename, buffer, &grepFinished);

    //sync status with client
    while( !grepFinished ){
        //cout<<"Server: grepping"<<endl;
        char running[3] = {1, 10, 1};
        ret = robustWrite(connFd, running, 3 );
        usleep( SLEEP_TIME * 1000 );
    }

    //finish grepping
    systemCmd.join();

    //check file exist
    struct stat st;
    if(stat( filename.c_str(), &st) != 0){
        cout<<"Server: cannot find file "<<filename<<endl;
        exit(0);
    }

    int fd = open(filename.c_str(), O_RDONLY);
    void * filep = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close( fd );

    buffer[0] = 0;  
    buffer[1] = 8;  
    buffer[2] = 8;
    ((int*)buffer)[1] = st.st_size;
    
    robustWrite(connFd, buffer, 8);

    robustRead(connFd, buffer, 8);

    splitWrite(connFd, (char*)filep, st.st_size);

    munmap( filep,  st.st_size );
    close(connFd);
    delete [] buffer;

    //cleanning tmp file on server
    string sysCmd = "rm ";
    sysCmd += filename;
    system(sysCmd.c_str());
    cout<<sysCmd<<endl;

    printf("Server: mission finished\n");
    
    return;

}

//Client Thread
void connection_thread(std::string input, std::string address, int serverPort, int threadId)
{
    int connectionToServer;

    connect_to_server(address.c_str(), serverPort, &connectionToServer);

    char* buffer = new char[BUFFER_MAX];
    int ret;
    char * cstr = new char [input.length()+1];
    strcpy (cstr, input.c_str());

    //send command
    ret = write(connectionToServer, cstr, input.length()+1 );

    //check status
    struct pollfd isReady;
    isReady.fd = connectionToServer;
    isReady.events = POLLIN;
    while(true){
        int ret = poll( &isReady, 1, MAX_LATENCY + SLEEP_TIME);
        if(ret==0) {
            cout<<"Clinet: the server is dead"<<endl;
            delete [] buffer;
            delete [] cstr;
            close(connectionToServer);
            return;
        }
        else{
            read(connectionToServer, buffer, BUFFER_MAX);
            //robustRead(connectionToServer, buffer, BUFFER_MAX);
            if(buffer[0]==1 && buffer[1]==10 && buffer[2]==1){
                //cout<<"Clinet: socket "<< connectionToServer <<" is running"<<endl;
                //usleep( SLEEP_TIME * 1000 );
                continue;
            }
            else
                break;
        }
    }

    int filesize = ((int*)buffer)[1];
    //cout<<"Client: filesize "<<filesize<<endl;

    write(connectionToServer, buffer, 8 );

    //memory buffer method
    char * result = new char[filesize];
    splitRead(connectionToServer, result, filesize );
    
    //erase file descriptor information
    result[ strlen(result) - 3 ] = 0;
    
    close(connectionToServer);
    delete [] buffer;

    printLogLock.lock();
    printLog( result, threadId );
    printLogLock.unlock();
    
    delete [] result;

    printf("Client %d: mission finished\n", threadId);

    return;
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

    getAdress("Address", 1);

    std::vector <std::thread> threads;

    for (int i = 0; i < NODES_NUMBER-1; ++i)
    {
        threads.push_back(std::thread(connection_thread, input, address.at(i), SERVER_PORT, i+1));
    }

    for (auto& th : threads) th.join();

    return;
}


int main (int argc, char* argv[])
{
    char a;
    bool flag;

    SERVER_PORT = atoi(argv[1]);

    std::cout << "Distributed Logging init." << std::endl;

    std::thread listeningServer(listeningThread, SERVER_PORT);
    std::cout << "Type 'grep' if you want to see logs: ";
    std::thread cinListening( listeningCin );

    cinListening.join();
    listeningServer.join();
    
    return 0;
}
