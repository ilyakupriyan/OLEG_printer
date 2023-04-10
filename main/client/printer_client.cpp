#include <iostream>
#include <../include/error.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

int test_sock(int sock);     /* Function for testing connection to server */

int main(void) 
{
    int             sock_server;            /* Pointer to socket descriptor */
    sockaddr_in     addr_server;            /* Structure with address */

    sock_server = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_server < 0)
    {
        perror("Socket clinet");
        exit(1);
    }

    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(3452);
    addr_server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sock_server, (sockaddr *) &addr_server, sizeof(addr_server)) < 0)
    {
        perror("Connect");
        exit(2);
    }

    test_sock(sock_server);
}

int test_sock(int sock) {
    char        mess[] = "Test socket!",
                rec_mess[sizeof(mess)];
    send(sock, mess, sizeof(mess), 0);
    recv(sock, rec_mess, sizeof(rec_mess), 0);
    if (strcmp(mess, rec_mess) == 0)
    {
        return 0;
    }
    return 1;
}