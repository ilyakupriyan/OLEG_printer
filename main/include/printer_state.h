/* This header file is intended for local error in printing system */

//state of destination
typedef enum printer_state_e
{
    PRINTER_WORK,                           //Printer is working
    PRINTER_IDLE,                           //Printer 
    PRINTER_STOPPED,
    PRINTER_ERROR_DISCONNECT_DEST,          //Disconnected printer error
    PRINTER_ERROR_OTHER                     //Other types error
} printer_state;

//Command to printer server
typedef enum server_command_e
{
    PRINTER_INFO,                           //Information about printer
    PRINT_FILE,                             //Print file 
    PRINT_INFO,                             //State of last job
    INFO_JOBS                               //State of jobs
} server_command;

typedef enum job_state_e
{
    JOB_PENDING,
    JOB_HELD,
    JOB_PROCESSING,
    JOB_STOPPED,
    JOB_CANCELED,
    JOB_ABORTED,
    JOB_COMPLETED
} job_state;