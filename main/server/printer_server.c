#include <stdio.h>
#include <cups/cups.h>
#include <string.h>
#include <errno.h>
#include "../include/printer_state.h"
#include "../include/cJSON.h"
#include <sys/types.h>
#include <sys/socket.h>

#define __DEBUG

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

const char *conf_file = "config.json";

int test_sock(int sock);                    /* Function for testing connection to client */
int test_JSON(cJSON* json_s);               /* Test function of parsing JSON */

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

    /* Reading file with JSON objects */        
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
    config = cJSON_Parse(JSON_str);     //Parsing JSON
    if (config == NULL) 
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "JSON error before: %s\n", error_ptr);
        }
        exit(1);
    }

    /* Getting destination */
    dest_name = cJSON_GetObjectItemCaseSensitive(config, "dest");
    dest_str = cJSON_Print(dest_name);
    dest = cupsGetDestWithURI(NULL, dest_str);
    while (dest == NULL) 
    {
        
    }

    /* Socket setting */
    if (listener = socket(AF_INET, SOCK_STREAM, 0))
    {
        perror("Socket server: ");
        exit(2);
    }
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(3452);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listener, (sockaddr *) &addr, sizeof(addr)) < 0)
    {
        perror("Server bind");
        exit(3);
    }

    listen(listener, 1); /*Setting socket to listening */

    for (int i = 1; test_sock(listener) == 0 && i <= 3; i++)
    {
        puts("Connection was not realised\n");
        sleep(2);
        {
            exit(4);
        }
    }

    while (1) 
    {  
        #ifdef __DEBUG
        #endif

        int command;

        /* Creating server socket */
        sock = accept(listener, NULL, NULL);  
        if (sock < 0) 
        {
            perror("Accept");
            exit(3);
        }
        bytes_read = recv(sock, &command, 1024, 0);

        if (command == PRINTER_INFO)
        {   
            char    *printer_st,
                    *printer_st_reasons;
            printer_st = cupsGetOption("printer-state", dest->num_options, dest->options);
            printer_st_reasons = cupsGetOption("printer_st_reasons", dest->num_options, dest->options);

            #ifdef __DEBUG
                printf("%s:\n", dest->name);
                printf("State: %s\n", printer_st);
                printf("State-reasons: %s\n", printer_st_reasons);
            #endif
        }
        
        close(sock);


    }
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