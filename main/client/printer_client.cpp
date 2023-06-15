#include <iostream>
#include "../include/printer_state.h"
#include <sys/types.h>
#include <cups/cups.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define __DEBUG

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

int test_sock(int sock);     /* Function for testing connection to server */

int main(void) 
{
    int             sock_server,            /* Pointer to socket descriptor */
                    comm_server,            /* Code of command to server */
                    answ_server;            /* Answer_from server */
    sockaddr_in     addr_server;            /* Structure with address */

    sock_server = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_server < 0)
    {
        perror("Socket client");
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

    for (int i = 1; test_sock(sock_server) == 0 && i <= 3; i++)
    {
        puts("Connection was not realised\n");
        sleep(2);
        {
            exit(3);
        }
    }
    #ifdef __DEBUG
    comm_server = PRINTER_INFO;
    #endif
    
    send(sock_server, &comm_server, sizeof(comm_server), 0);                    //sending command to server
    recv(sock_server, (int *) &answ_server, sizeof(answ_server), 0);
    
    if (comm_server == PRINTER_INFO) 
    {
        switch (answ_server) 
        {
            case PRINTER_WORK:
            case PRINTER_IDLE:
                #ifdef __DEBUG
                printf ("Printer state: %d\n", answ_server);
                #endif
            break;
            case PRINTER_STOPPED:
                #ifdef __DEBUG
                printf ("Printer-state-reasons: %d\n", answ_server);
                #endif
            break;
            default:

            break;
        }
    }
    
}



////////////////////////
//  Function test block
////////////////////////
int test_sock(int sock) {
    char        mess[] = "Test!",
                rec_mess[sizeof(mess)];
    send(sock, mess, sizeof(mess), 0);
    recv(sock, rec_mess, sizeof(rec_mess), 0);
    if (strcmp(mess, rec_mess) == 0)
    {
        return 0;
    }
    return 1;
}