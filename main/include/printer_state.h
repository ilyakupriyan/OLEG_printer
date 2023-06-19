/* This header file is intended for local error in printing system */

//state of destination
typedef enum printer_state_e
{
    PRINTER_WORK = 3,                       //Printer is working
    PRINTER_IDLE,                           //Printer is idle
    PRINTER_STOPPED,                        //Printer is stopped
} printer_state_t;

typedef enum printer_state_reason_e
{
    CONNECTING_TO_DEVICE = 6,               //Connecting to printer but not printing yet
    COVER_OPEN,                             //The printer's cover is open
    INPUT_TRAY_MISSING,                     //The paper tray is missing
    MARKER_SUPPLY_EMPTY,                    //The printer is out of ink
    MARKER_SUPPLY_LOW,                      //The printer is almost out of ink
    MARKER_WASTE_ALMOST_FULL,               //The printer's waste bin is almost full
    MARKER_WASTE_FULL,                      //The printer's waste bin is full
    MEDIA_EMPTY,                            //The paper tray (any paper tray) is empty
    MEDIA_JAM,                              //There is a paper jam
    MEDIA_LOW,                              //The paper tray (any paper tray) is almost empty
    MEDIA_NEEDED,                           //The paper tray needs to be filled (for a job that is printing)
    PAUSED,                                 //Stop the printer
    TIMED_OUT,                              //Unable to connect to printer
    TONER_EMPTY,                            //The printer is out of toner
    TONER_LOW                               //The printer is low on toner
} printer_state_reason_t;

//Command to printer server
typedef enum server_command_e
{
    PRINTER_INFO,                           //Information about printer
    PRINT_FILE,                             //Print file 
    PRINT_INFO,                             //State of last job
    INFO_JOB                                //State of job
} server_command_t;

typedef enum job_state_e
{
    JOB_PENDING,
    JOB_HELD,
    JOB_PROCESSING,
    JOB_STOPPED,
    JOB_CANCELED,
    JOB_ABORTED,
    JOB_COMPLETED
} job_state_t;