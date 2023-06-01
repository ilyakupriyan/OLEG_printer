#include <stdio.h>
#include <cups/cups.h>
#include <string.h>
#include <errno.h>
#include "../include/error.h"
#include "../include/cJSON.h"
#include <sys/types.h>
#include <sys/socket.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

const char *conf_file = "conf.json";

int test_sock(int sock);                  /* Function for testing connection to client */

int main(void) 
{
    int                 sock,               /* Pointer to socket descriptor*/
                        listener,           /* Pointer to listening socket */
                        bytes_read;         /* Number bytes, which will be reading */
    sockaddr_in         addr;               /* Structure with data of adress */
    char               *JSON_str,           /* Text from file conf.JSON*/
                       *dest_str;           /* Destination in string format */
    cJSON              *config,             /* JSON file with configuration */
                       *ipp;                /* String with ipp destination */
    const cJSON        *dest_name = NULL;   /* destination name in JSON */
    FILE               *conf_fp;            /* Pointer to file desrciptor  */
    cups_dest_t        *dest;               /* Destination for printing */
    printer_error       error;              /* System errors */

    /* Getting JSON objects */        
    conf_fp = fopen(conf_file, "r");
    do 
    {
        char *buf;
        fgets(buf, 512, conf_fp);
        strcat(JSON_str, buf);
        if (strrchr(buf, EOF) == NULL)
        {
            break;
        }   
    } 
    while (1);
    fclose(conf_file);
    config = cJSON_Parse(JSON_str);

    /* Getting destination */
    dest_name = cJSON_GetObjectItemCaseSensitive(config, "dest");
    dest_str = cJSON_Print(dest_name);
    dest = cupsGetDestWithURI(NULL, dest_str);
    while (dest == NULL) 
    {
        
    }

    if (listener = socket(AF_INET, SOCK_STREAM, 0))
    {
        perror("Socket server");
        exit(1);
    }

    /* Setting of connection properties */
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(3452);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listener, (sockaddr *) &addr, sizeof(addr)) < 0)
    {
        perror("Bind");
        exit(2);
    }

    listen(listener, 1); /*Setting socket to listening */

    for (int i = 1; test_sock(listener) == 0 && i <= 3; i++)
    {
        puts("Connection was not realised\n");
        sleep(2);
        {
            exit(3);
        }
    }

    // while (1) 
    // {
    //     /* Creating server socket */
    //     sock = accept(listener, NULL, NULL);  
    //     if (sock < 0) 
    //     {
    //         perror("Accept");
    //         exit(3);
    //     }

    //     bytes_read = recv(sock, data, 1024, 0);
        
    //     close(sock);
    // }
}


////////////////////////
//  Funsction test block
////////////////////////
int test_sock(int listener) {
    char    *sample = "Test!",
            *data;
    int      socket,
             bytes_read;

    socket = accept(listener, NULL, NULL);

    bytes_read = recv(socket, data, sizeof(sample), 0);
    if (strcmp(sample, data) == 0)
    {
        send(socket, sample, sizeof(sample), 0);
        close(socket);
        return 0;
    }
    close(socket);
    return 1;
}

