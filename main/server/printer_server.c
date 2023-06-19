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

#ifdef __DEBUG
const char *conf_file = "config.json";
#endif

int test_sock(int sock);                                                    
cJSON* readingJSON(const char *JSON_file_ptr);                              
printer_state_reason_t getStateReason (const char *state_reasons_str);  

int main(int argc, char *argv[]) 
{
    int                 sock,               /* Pointer to socket descriptor*/
                        listener,           /* Pointer to listening socket */
                        bytes_read;         /* Number bytes, which will be reading */
    sockaddr_in         addr;               /* Structure with data of adress */
    char               *JSON_str,           /* Text from file conf.JSON*/
                       *dest_URI_str;           /* Destination in string format */
    cJSON              *config,             /* JSON file with configuration */
                       *ipp;                /* String with ipp destination */
    const cJSON        *dest_URI_name = NULL;   /* destination name in JSON */
    FILE               *conf_fp;            /* Pointer to file desrciptor  */
    cups_dest_t        *dest;               /* Destination for printing */

    /* Reading file with JSON objects */   
    #ifdef __DEBUG
    config = readingJSON(conf_file);
    #endif

    #ifndef __DEBUG
    config = readingJSON(argv[1]);
    #endif

    if (config == NULL) 
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "JSON error before: %s\n", error_ptr);
        }
        else
        {
            fprintf (stderr, "Error: couldn't get JSON\n");
        }
        exit(1);
    }

    /* Getting destination */
    dest_URI_name = cJSON_GetObjectItemCaseSensitive(config, "IPP");
    dest_URI_str = cJSON_Print(dest_URI_name);
    dest = cupsGetDestWithURI(NULL, dest_URI_str);
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
            exit(5);
        }
        bytes_read = recv(sock, &command, 1024, 0);

        /* */
        if (command == PRINTER_INFO)
        {   
            const char      *printer_st_str,
                            *printer_st_reasons_str;
            int              printer_st_i,
                             printer_st_reasons_i;

            printer_st_str = cupsGetOption("printer-state", dest->num_options, dest->options);
            printer_st_reasons_str = cupsGetOption("printer-state-reasons", dest->num_options, dest->options);

            #ifdef __DEBUG
                printf("%s:\n", dest->name);
                printf("State: %s\n", printer_st_str);
                printf("State-reasons: %s\n", printer_st_reasons_str);
            #endif

            printer_st_i = atoi(printer_st_str);

            //Proccessing the printer state
            switch (printer_st_i) {
                case PRINTER_WORK:
                case PRINTER_IDLE:
                    send(sock, (int *) &printer_st_i, sizeof(printer_st_i), 0);
                break;
                case PRINTER_STOPPED:
                    printer_st_reasons_i = getStateReason(printer_st_reasons_str);
                    send(sock, (int *) printer_st_reasons_i, sizeof(printer_st_reasons_i), 0);
                break; 
            }
        }
        else if (command == PRINT_FILE) 
        {
            int path_file_size = 0;             //count of symbol in file path
            bytes_read = recv(sock, (int *) &path_file_size, sizeof(path_file_size), 0);

            char *path_file = calloc (path_file_size, sizeof(char));
            bytes_read = recv(sock, (char *) path_file, path_file_size, 0);

        }
        else if (command == PRINT_INFO)
        {
            
        }
        else if (command == INFO_JOB)
        {

        }

        close(sock);
    }
}

/*
 * readingJSON - Read text file for converting to JSON format
 * O (cJSON *) - pointer to JSON
 * I (const char *JSON_file_ptr) - pointer to text file with JSON 
 */
cJSON* readingJSON(const char *JSON_file_ptr)          
{
    FILE * JSON_fp;
    char * JSON_str;
    JSON_fp = fopen(JSON_file_ptr, "r");
    if (JSON_fp == NULL)
    {
        return 0;
    }

    do 
    {
        char *buf;
        fgets(buf, 512, JSON_fp);
        strcat(JSON_str, buf);
        if (strrchr(buf, EOF) == NULL)
        {
            break;
        }   
    } 
    while (1);
    fclose(JSON_fp);
    return cJSON_Parse(JSON_str);     //Parsing JSON
}

/*
 * getStateReason - Convert from string state reasons to printer_state_reason_t
 * O (printer_state_reason_t) - printer state reason in the enum format
 * I (const char *state_reason) - printer state reason in string format
 */
printer_state_reason_t getStateReason (const char *state_reasons_str)
{
    printer_state_reason_t state_reason_value;

    if (!strcmp(state_reasons_str, "connecting-to-device"))
        state_reason_value = CONNECTING_TO_DEVICE;
    else if (!strcmp(state_reasons_str, "cover-open"))
        state_reason_value = COVER_OPEN;
    else if (!strcmp(state_reasons_str, "input-tray-missing"))
        state_reason_value = INPUT_TRAY_MISSING;  
    else if (!strcmp(state_reasons_str, "marker-supply-empty"))
        state_reason_value = MARKER_SUPPLY_EMPTY;
    else if (!strcmp(state_reasons_str, "marker-supply-low"))
        state_reason_value = MARKER_SUPPLY_LOW;
    else if (!strcmp(state_reasons_str, "marker-waste-almost-full"))
        state_reason_value = MARKER_WASTE_ALMOST_FULL;
    else if (!strcmp(state_reasons_str, "marker-waste-full"))
        state_reason_value = MARKER_WASTE_FULL;
    else if (!strcmp(state_reasons_str, "media-empty"))
        state_reason_value = MEDIA_EMPTY;
    else if (!strcmp(state_reasons_str, "media-jam"))
        state_reason_value = MEDIA_JAM;
    else if (!strcmp(state_reasons_str, "media-low"))
        state_reason_value = MEDIA_LOW;
    else if (!strcmp(state_reasons_str, "marker-needed"))
        state_reason_value = MEDIA_NEEDED;
    else if (!strcmp(state_reasons_str, "paused"))
        state_reason_value = PAUSED;
    else if (!strcmp(state_reasons_str, "timed-out"))
        state_reason_value = TIMED_OUT;
    else if (!strcmp(state_reasons_str, "toner-empty"))
        state_reason_value = TONER_EMPTY;
    else if (!strcmp(state_reasons_str, "toner-low"))
        state_reason_value = TONER_LOW;

    return state_reason_value;
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