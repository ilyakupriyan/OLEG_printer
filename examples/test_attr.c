#include <stdio.h>
#include <cups/cups.h>
#include <errno.h>

static void	print_file(http_t *http, cups_dest_t *dest, cups_dinfo_t *dinfo, const char *filename, int num_options, cups_option_t *options);
ipp_t *getPrinterAttr(cups_dest_t *dest);

int main (int argc, char *argv[]) {

    http_t	        *http;			/* Connection to destination */
    cups_dest_t	    *dest = NULL;	/* Destination */
    cups_dinfo_t	*dinfo;			/* Destination info */


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
                    int			      i,		/* Looping var */
                        num_options = 0;        /* Number of options */
        cups_option_t	*options = NULL;        /* Options */
        
        for (i = 4; i < argc; i ++)
            num_options = cupsParseOptions(argv[i], num_options, &options);
        print_file(http, dest, dinfo, argv[3], num_options, options);
    }

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

static void print_file( http_t        *http,		/* I - Connection to destination */
                        cups_dest_t   *dest,		/* I - Destination */
	                    cups_dinfo_t  *dinfo,	    /* I - Destination information */
                        const char    *filename,	/* I - File to print */
	                    int           num_options,	/* I - Number of options */
	                    cups_option_t *options)	    /* I - Options */
{  

    cups_file_t            *fp;			/* File to print */
    int                 job_id;			/* Job ID */
    ipp_status_t	    status;		    /* Submission status */
    const char	        *title;			/* Title of job */
    char		        buffer[32768];	/* File buffer */
    ssize_t         	bytes;			/* Bytes read/to write */

    if ((fp = cupsFileOpen(filename, "r")) == NULL) 
    {
        printf("Unable to open \"%s\": %s\n", filename, strerror(errno));
        return;
    }
    if ((title = strrchr(filename, '/')) != NULL)
        title++;
    else
        title = filename;
    if ((status = cupsCreateDestJob(http, dest, dinfo, &job_id, title, num_options, options)) > IPP_STATUS_OK_IGNORED_OR_SUBSTITUTED) 
    {
        printf("Unable to create job: %s\n", cupsLastErrorString());
        cupsFileClose(fp);
        return;
    }

    printf("Created job ID: %d\n", job_id);

    if (cupsStartDestDocument(http, dest, dinfo, job_id, title, CUPS_FORMAT_AUTO, 0, NULL, 1) != HTTP_STATUS_CONTINUE) 
    {IPP_TAG_OPERATIONsFileClose(fp);
        return;
    }
    while ((bytes = cupsFileRead(fp, buffer, sizeof(buffer))) > 0) 
    {
        if (cupsWriteRequestData(http, buffer, (size_t)bytes) != HTTP_STATUS_CONTINUE) 
        {
            printf("Unable to write document data: %s\n", cupsLastErrorString());
            break;
        }
    }
    cupsFileClose(fp);
    if ((status = cupsFinishDestDocument(http, dest, dinfo)) > IPP_STATUS_OK_IGNORED_OR_SUBSTITUTED) 
    {
        printf("Unable to send document: %s\n", cupsLastErrorString());
        return;
    }
    puts("Job queued.");
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