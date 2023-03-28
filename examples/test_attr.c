#include <stdio.h>
#include <cups/cups.h>
#include <errno.h>

static void	printFile(http_t *http, cups_dest_t *dest, cups_dinfo_t *dinfo, const char *filename, int num_options, cups_option_t *options);
ipp_t *getPrinterAttr(cups_dest_t *dest);

int main (int argc, char *argv[]) {

    http_t	        *http;			/* Connection to destination */
    cups_dest_t	    *dest = NULL;	/* Destination */
    cups_dinfo_t	*dinfo;			/* Destination info */
    int job_id;                     /* ID of job*/


    if (!strncmp(argv[1], "ipp://", 6) || !strncmp(argv[1], "ipps://", 7))
        dest = cupsGetDestWithURI(NULL, argv[1]);
    else
        dest = cupsGetNamedDest(CUPS_HTTP_DEFAULT, argv[1], NULL);

    if (!dest) 
    {
        printf("print: Unable to get destination \"%s\": %s\n", argv[1], cupsLastErrorString());
        return 1;
    }

    if ((http = cupsConnectDest(dest, CUPS_DEST_FLAGS_NONE, 30000, NULL, NULL, 0, NULL, NULL)) == NULL) 
    {
        printf("print: Unable to connect to destination \"%s\": %s\n", argv[1], cupsLastErrorString());
        return 1;
    }

    if ((dinfo = cupsCopyDestInfo(http, dest)) == NULL) 
    {
        printf("print: Unable to get information for destination \"%s\": %s\n", argv[1], cupsLastErrorString());
        return 1;
    }

    if (!strcmp(argv[2], "print") && argc > 3)  /*print option*/
    {
                int			          i,		/* Looping var */
                        num_options = 0,        /* Number of options */
                               num_jobs;        /* destination number of jobs */
        cups_option_t	*options = NULL;        /* Options */
        char           file_print[1024];     
        ipp_status_t             status;   
        ipp_jstate_t          job_state = IPP_JOB_PENDING;
        cups_job_t                *jobs;        /* Jobs of destination */


        for (i = 4; i < argc; i ++)
            num_options = cupsParseOptions(argv[i], num_options, &options);
        job_id = cupsPrintFile(dest->name, argv[3], "Test print", num_options, options);


        if ((status = cupsFinishDestDocument(http, dest, dinfo)) > IPP_STATUS_OK_IGNORED_OR_SUBSTITUTED) 
        {
            printf("Unable to send document: %s\n", cupsLastErrorString());
            return;
        }

        while (job_state < IPP_JOB_STOPPED)
        {

            num_jobs = cupsGetJobs(&jobs, dest->name, 1, CUPS_WHICHJOBS_ALL);

            /* Selection of job state in destination */
            job_state = jobs[job_id].state;

            /* Show the current state */
            switch (job_state)
            {
                case IPP_JOB_PENDING :
                    printf("Job %d is pending.\n", job_id);
                    break;
                case IPP_JOB_HELD :
                    printf("Job %d is held.\n", job_id);
                    break;
                case IPP_JOB_PROCESSING :
                    printf("Job %d is processing.\n", job_id);
                    break;
                case IPP_JOB_STOPPED :
                    printf("Job %d is stopped.\n", job_id);
                    break;
                case IPP_JOB_CANCELED :
                    printf("Job %d is canceled.\n", job_id);
                    break;
                case IPP_JOB_ABORTED :
                    printf("Job %d is aborted.\n", job_id);
                    break;
                case IPP_JOB_COMPLETED :
                    printf("Job %d is completed.\n", job_id);
                    break;
            }

            /* Sleep if the job is not finished */
            if (job_state < IPP_JOB_STOPPED)
                sleep(5);
        }
    }

/*
 *   Part of printer response    
 */

    /*Start setting IPP request*/
    ipp_attribute_t *attr;
    ipp_t *response = getPrinterAttr(dest);

    if ((attr = ippFindAttribute(response, "printer-state", IPP_TAG_ENUM)) != NULL)
    {
        printf("printer-state=%s\n", ippEnumString("printer-state", ippGetInteger(attr, 0)));
    }
    else
        puts("printer-state=unknown");

    if ((attr = ippFindAttribute(response, "printer-state-message", IPP_TAG_TEXT)) != NULL)
    {
        printf("printer-state-message=\"%s\"\n", ippGetString(attr, 0, NULL));
    }

    if ((attr = ippFindAttribute(response, "printer-state-reasons", IPP_TAG_KEYWORD)) != NULL)
    {
        int i, count = ippGetCount(attr);
        puts("printer-state-reasons=");
        for (i = 0; i < count; i ++)
            printf("    %s\n", ippGetString(attr, i, NULL));
    }

    ippDelete(response);
    return 0;
}

/*
 * get_printer_attr - function for getting attributes of printer
 * O (ipp_t *) - response
 */
ipp_t *getPrinterAttr(cups_dest_t *dest_goal)  /* I - destination (printer)*/
{
    ipp_t *request = ippNewRequest(IPP_OP_GET_PRINTER_ATTRIBUTES);
    const char *printer_uri = cupsGetOption("device-uri", dest_goal->num_options, dest_goal->options);
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri", NULL, printer_uri);

    return (cupsDoRequest(CUPS_HTTP_DEFAULT, request, "/"));
} 