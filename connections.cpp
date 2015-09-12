#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "connections.h"


/* Server operation */
int open_socket(int port)
{
    socklen_t len; //store size of the address
    struct sockaddr_in svrAdd, clntAdd;
    int listenFd;
    int connFd;
    
    //create socket
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(listenFd < 0)
    {
        std::cout << "Cannot open socket" << std::endl;
        exit(1);
    }
    
    memset((char*) &svrAdd,0, sizeof(svrAdd));
    
    svrAdd.sin_family = AF_INET;
    svrAdd.sin_addr.s_addr = INADDR_ANY;
    svrAdd.sin_port = htons(port);
    
    //bind socket
    if(bind(listenFd, (struct sockaddr *)&svrAdd, sizeof(svrAdd)) < 0)
    {
        std::cout << "open_socket: Cannot bind" << std::endl;
        exit(1);
    }

    
    listen(listenFd, 5);
    
    len = sizeof(clntAdd);
    
    int noThread = 0;

    //std::cout << "Listening in background... " << std::endl;

    printf("Listening at port %d...\n", port);

    //this is where client connects. svr will hang in this mode until client conn
    connFd = accept(listenFd, (struct sockaddr *)&clntAdd, &len);

    if (connFd < 0)
    {
        std::cout << "Cannot accept connection" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "Server Connection successful " << std::endl;
    }

    return connFd;
}

/*Client operation */
void connect_to_server(const char* add, int port, int* connectionFd)
{
    struct sockaddr_in svrAdd;
    struct hostent *server;
    
    if((port > 65535) || (port < 2000))
    {
        std::cout<<"Please enter port number between 2000 - 65535"<<std::endl;
        exit(1);
    }       
    
    //create client skt
    *connectionFd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(*connectionFd < 0)
    {
        std::cout << "Cannot open socket" << std::endl;
        exit(1);
    }
    else{
        std::cout << "Socket opened" << std::endl;
    }
    
    server = gethostbyname(add);
    
    if(server == NULL)
    {
        std::cout << "Host does not exist" << std::endl;
        exit(1);
    }
    
    memset((char *) &svrAdd,0, sizeof(svrAdd));
    svrAdd.sin_family = AF_INET;
    
    memcpy((char *) &svrAdd.sin_addr.s_addr,(char *) server -> h_addr, server -> h_length);
    
    svrAdd.sin_port = htons(port);
    
    int checker = connect(*connectionFd,(struct sockaddr *) &svrAdd, sizeof(svrAdd));
    
    if (checker < 0)
    {
        std::cout << "Cannot connect to: " << add << std::endl;
        //exit(1);
    }
    else{
        std::cout << "Client Connection Successful" << std::endl;
    }
}
