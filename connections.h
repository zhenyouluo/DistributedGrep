#ifndef CONNECTIONS_H
#define CONNECTIONS_H

/* Server operation */
int open_socket(int port);

/*Client operation */
void connect_to_server(const char* add, int port, int* connectionFd);

#endif