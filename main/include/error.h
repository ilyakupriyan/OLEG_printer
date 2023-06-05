/* This header file is intended for local error in printing system */

//destination error
typedef enum printer_error_e
{
    ERROR_DISCONNECT_DEST,          //Disconnected printer error
    OTHER_ERROR                     //Other types error
} printer_error;