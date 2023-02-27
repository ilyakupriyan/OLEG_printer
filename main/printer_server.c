#include <stdio.h>
#include <cups/cups.h>
#include <errno.h>

static void	print_file(http_t *http, cups_dest_t *dest, cups_dinfo_t *dinfo, const char *filename, int num_options, cups_option_t *options);

static void print_file( http_t        *http,		/* I - Connection to destination */
                        cups_dest_t   *dest,		/* I - Destination */
	                    cups_dinfo_t  *dinfo,	    /* I - Destination information */
                        const char    *filename,	/* I - File to print */
	                    int           num_options,	/* I - Number of options */
	                    cups_option_t *options)	    /* I - Options */
{  

    cups_file_t	        *fp;			/* File to print */
    int		            job_id;			/* Job ID */
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
    {
        printf("Unable to send document: %s\n", cupsLastErrorString());
        cupsFileClose(fp);
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