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

using namespace std;

#define BUFFER_MAX (1024*1024*8)
#define NODES_NUMBER (3)

//  in ms
#define SLEEP_TIME (50)
#define MAX_LATENCY (2000)

std::mutex printLogLock;

int SERVER_PORT;

std::vector<std::stringstream> logs(NODES_NUMBER);
std::vector<std::string> address;

class message{
public:
    message(){ begin=0; length=BUFFER_MAX; };
    int begin;
    int length;
};

void printLog(char * result, int threadId){
    printf("\n%s\n", result);
    return;
}

int robustRead(int connFd, char * buffer, int length){
    int count = 0, add = 0;
    while(count < length){
        if( (add = read(connFd, buffer, length)) > 0 ){
            count += add;
            buffer += add;
        }
        else if( add < 0 ){
            return -10;
        }
        else if(add == 0){
            break;
        }
    }
    return count;
}

int robustWrite(int connFd, char * buffer, int length){
    int count = 0, add = 0;
    while(count < length){
        if( ( add = write(connFd, buffer, length)) > 0 ){
            count += add;
            buffer += add;
        }
        else if( add < 0 ){
            return -1;
        }
    }
    return count;
}

//write large package on socket
int splitWrite( int connFd, char * data, int size ){
    
    int msg_num = size / BUFFER_MAX + 1;
    int rest = size % ((int)BUFFER_MAX);
    
    //store the should-be message sizes
    message *msgs = new message[msg_num];
    for(int i=1; i < msg_num; i++){
        msgs[i].begin = msgs[i-1].begin + BUFFER_MAX;
    }
    msgs[msg_num-1].length = rest;

    char writeSuccess = 0;
    for(int i=0; i < msg_num; i++){
        //write each message, max size is BUFFER_MAX
        robustWrite( connFd, data + msgs[i].begin, msgs[i].length );
        
        //cout<<"Server: sent " << msgs[i].length << endl;
        //usleep(200*1000);
        
        //make sure client do got the data
        robustRead( connFd, &writeSuccess, 1 );
        
        //cout<<"Server: recv "<<(int)writeSuccess<<endl;
        //usleep(200*1000);

        //if package failed, send again
        if(writeSuccess==0) i--;
    }
    return size;
}

int splitRead( int connFd, char * data, int size ){
    
    int msg_num = size / BUFFER_MAX + 1;
    int rest = size % ((int)BUFFER_MAX);
    
    //store the should-be message sizes
    message *msgs = new message[msg_num];
    for(int i=1; i < msg_num; i++){
        msgs[i].begin = msgs[i-1].begin + BUFFER_MAX;
    }
    msgs[msg_num-1].length = rest;

    char readSuccess = 0;
    int readSize;

    for(int i=0; i < msg_num; i++){

        //read message from socket. max size BUFFER_MAX
        readSize = robustRead( connFd, data + msgs[i].begin, msgs[i].length );
        
        //cout<<"client recv size :" << readSize << endl;
        //usleep(200*1000);

        //check if reading success or not
        if(readSize == msgs[i].length)  readSuccess = 1;
        else    readSuccess = 0;

        //tell server the data transmission status (good or fail)
        robustWrite( connFd, &readSuccess, 1);
        
        //cout<<"Client: sent "<<endl;
        //usleep(200*1000);
        
        //if reading faile, read again
        if(readSuccess==0) i--;
    }
    return size;
}

//Server Thread
void listeningThread(int serverPort)
{   
    //set connection
    int listenFd = open_socket(serverPort);
    while(true)
    {

        int ret;
        
        int connFd = listen_socket(listenFd);

        //recv command from client
        char* buffer = new char[BUFFER_MAX];
        ret = read (connFd, buffer, BUFFER_MAX);
        
        //get temp output filename
        string filename;
        stringstream ss;
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

        /*
        //check file exist
        struct stat st;
        if(stat( filename.c_str(), &st) != 0){
            cout<<"Server: cannot find file "<<filename<<endl;
            exit(0);
        }

        int fd = open(filename.c_str(), O_RDONLY);
        void * filep = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        close( fd );
        */

        FILE* f = fopen(filename.c_str(), "r");
        fseek(f, 0, SEEK_END);
        size_t filesize = ftell(f);

        char *filep = new char[filesize];
        rewind(f);
        fread(filep, sizeof(char), filesize, f);

        buffer[0] = 0;
        buffer[1] = 8;
        buffer[2] = 8;
        ((int*)buffer)[1] = filesize;
        //cout<<"Server: filesize: "<<st.st_size<<endl;

        robustWrite(connFd, buffer, 8);

        robustRead(connFd, buffer, 8);

        splitWrite(connFd, (char*)filep, filesize);
       

        close(connFd);
        delete [] buffer;
        delete [] filep;

        //cleanning tmp file on server
        string sysCmd = "rm ";
        sysCmd += filename;
        system(sysCmd.c_str());

        //printf("Server: The mission was a complete success!\n");
    }

    return;
}

//Client Thread
void connection_thread(std::string input, std::string address, int serverPort, int threadId)
{
    int connectionToServer;

    int ret = connect_to_server(address.c_str(), serverPort, &connectionToServer);
    if (ret != 0)
    {
        printf("Client: cannot connect to server: %s \n", address.c_str());
        return;
    }
    char* buffer = new char[BUFFER_MAX];

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
            printf("Client: the server %s is dead \n",address.c_str() );
            //exit(0);
            return;
            //kent
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
    
    close(connectionToServer);
    delete [] buffer;

    printLogLock.lock();
    printf("Logs from Machine %s: \n", address.c_str());
    printLog( result, threadId );
    printLogLock.unlock();
    
    delete [] result;

    //printf("Client %d: mission finished\n", threadId);

    return;
}

void getAdress(std::string filename)
{
    ifstream addFile(filename);

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
    while (true)
    {
        std::string input;
        int connectionToServer;
        std::cout << "Type 'grep' if you want to see logs: ";
        getline(std::cin, input);
        std::cout << "You entered: " << input << std::endl;

        if (input.compare("exit") == 0)
        {
            std::cout << "Exiting normally " << std::endl;
            exit(0);
        }

        getAdress("Address.add");

        std::vector <std::thread> threads;

        printf("Retriving information from remote machines... \n");

        for (int i = 0; i < NODES_NUMBER-1; ++i)
        {
            threads.push_back(std::thread(connection_thread, input, address.at(i), SERVER_PORT, i+1));
        }

        for (auto& th : threads) th.join();

        printf("Client: mission completed!\n");
    }

    return;
}


int main (int argc, char* argv[])
{
    char a;
    bool flag;

    SERVER_PORT = atoi(argv[1]);

    std::cout << std::endl << "CS425 - MP1: Distributed Logging init." << std::endl;

    std::thread listeningServer(listeningThread, SERVER_PORT);
    usleep(700);

    std::thread cinListening( listeningCin );

    cinListening.join();
    listeningServer.join();

    return 0;
}
