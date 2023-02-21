//Формат листов А3
//Расширения файлов, доступных для печати: pdf, jpeg
#include <stdio.h>
#include <cups/cups.h>
#include <string.h>
#include <errno.h>

static int	enum_cb (void *user_data, unsigned flags, cups_dest_t *dest);
static void usage (const char *arg) __attribute__((noreturn));
static void	print_file(http_t *http, cups_dest_t *dest, cups_dinfo_t *dinfo, const char *filename, int num_options, cups_option_t *options);

int main(int argc, char *argv[]) {

    http_t	        *http;			/* Connection to destination */
    cups_dest_t	    *dest = NULL;	/* Destination */
    cups_dinfo_t	*dinfo;			/* Destination info */

    if (argc < 2) {
        usage(NULL);
    }

    if (!strcmp(argv[1], "--enum")) {
        int                 i,              //переменная для for
                            flag;           //флаг для  cupsEnumDests()
        cups_ptype_t        type = 0,       //фильтр типа принтера
                            mask = 0;       //маска типа принтера

        for (i = 2; i < argc; i++) {
            if (!strcmp(argv[i], "grayscale")) {
                type |= CUPS_PRINTER_BW;
	            mask |= CUPS_PRINTER_BW;
            }
            else if (!strcmp(argv[i], "color")) {
                type |= CUPS_PRINTER_COLOR;
	            mask |= CUPS_PRINTER_COLOR;
            }
            else if (!strcmp(argv[i], "duplex")) {
                type |= CUPS_PRINTER_DUPLEX;
	            mask |= CUPS_PRINTER_DUPLEX;
            }
            else if (!strcmp(argv[i], "staple")) {
                type |= CUPS_PRINTER_STAPLE;
	            mask |= CUPS_PRINTER_STAPLE;
            }
            else if (!strcmp(argv[i], "small")) {
                type |= CUPS_PRINTER_SMALL;
	            mask |= CUPS_PRINTER_SMALL;
            }
            else if (!strcmp(argv[i], "medium")) {
                type |= CUPS_PRINTER_MEDIUM;
	            mask |= CUPS_PRINTER_MEDIUM;
            }
            else if (!strcmp(argv[i], "large")) {
                type |= CUPS_PRINTER_LARGE;
	            mask |= CUPS_PRINTER_LARGE;
            }
            else if (!strcmp(argv[i], "default")) {
                const char *dest_def = cupsGetDefault();
                cups_dest_t *def = cupsGetNamedDest(CUPS_HTTP_DEFAULT, dest_def, NULL);
                enum_cb(NULL, CUPS_DEST_FLAGS_NONE, def);
                flag = 1;
            }
            else {
                usage(argv[i]);
            } 
        }
        if (!flag)
            cupsEnumDests(CUPS_DEST_FLAGS_NONE, 5000, NULL, type, mask, enum_cb, NULL);

        return 0;
    }   //конец обработки опции --enum
    else if (!strcmp(argv[1], "--set")) {   //опция по работе с --set

        if (argc < 3) {
            usage(argv[1]);
        }

        if (!strcmp(argv[2], "dest-default")) {
            cups_dest_t *dest = cupsGetNamedDest(CUPS_HTTP_DEFAULT, argv[3], NULL); 
            if (dest == NULL) {
                printf ("Cannot find the \"%s\" defenition", argv[3]);
            }
            cupsSetDefaultDest(argv[3], NULL, 0, dest);
        }

    }
    else if (!strncmp(argv[1], "ipp://", 6) || !strncmp(argv[1], "ipps://", 7))
        dest = cupsGetDestWithURI(NULL, argv[1]);
    else
        dest = cupsGetNamedDest(CUPS_HTTP_DEFAULT, argv[1], NULL);

    
    return 0;
}

/*
 * 'enum_cb()'          - print the results from the enumeration of destinations.S
 */
static int enum_cb (void *user_data,    /* I - user data (unused) */
                unsigned flags,         /* I - flags */
             cups_dest_t *dest) {       /* I - destination */

    int	i;				/* Looping var */

    (void)user_data;
    (void)flags;

    if (dest->instance)
        printf("%s/%s:\n", dest->name, dest->instance);
    else
        printf("%s:\n", dest->name);
    
    for (i = 0; i < dest->num_options; i++)
        printf("    %s=\"%s\"\n", dest->options[i].name, dest->options[i].value);
    return 1;
}

static void print_file( http_t        *http,		/* I - Connection to destination */
                        cups_dest_t   *dest,		/* I - Destination */
	                    cups_dinfo_t  *dinfo,	    /* I - Destination information */
                        const char    *filename,	/* I - File to print */
	                    int           num_options,	/* I - Number of options */
	                    cups_option_t *options)	 {  /* I - Options */

    cups_file_t	        *fp;			/* File to print */
    int		            job_id;			/* Job ID */
    ipp_status_t	    status;		    /* Submission status */
    const char	        *title;			/* Title of job */
    char		        buffer[32768];	/* File buffer */
    ssize_t         	bytes;			/* Bytes read/to write */

    if ((fp = cupsFileOpen(filename, "r")) == NULL) {
        printf("Unable to open \"%s\": %s\n", filename, strerror(errno));
        return;
    }
    if ((title = strrchr(filename, '/')) != NULL)
        title++;
    else
        title = filename;
    if ((status = cupsCreateDestJob(http, dest, dinfo, &job_id, title, num_options, options)) > IPP_STATUS_OK_IGNORED_OR_SUBSTITUTED) {
        printf("Unable to create job: %s\n", cupsLastErrorString());
        cupsFileClose(fp);
        return;
    }

    printf("Created job ID: %d\n", job_id);

    if (cupsStartDestDocument(http, dest, dinfo, job_id, title, CUPS_FORMAT_AUTO, 0, NULL, 1) != HTTP_STATUS_CONTINUE) {
        printf("Unable to send document: %s\n", cupsLastErrorString());
        cupsFileClose(fp);
        return;
    }
    while ((bytes = cupsFileRead(fp, buffer, sizeof(buffer))) > 0) {
        if (cupsWriteRequestData(http, buffer, (size_t)bytes) != HTTP_STATUS_CONTINUE) {
            printf("Unable to write document data: %s\n", cupsLastErrorString());
            break;
        }
    }
    cupsFileClose(fp);
    if ((status = cupsFinishDestDocument(http, dest, dinfo)) > IPP_STATUS_OK_IGNORED_OR_SUBSTITUTED) {
        printf("Unable to send document: %s\n", cupsLastErrorString());
        return;
    }
    puts("Job queued.");
}


static void usage(const char *arg)	{		/* I - Argument for usage message */
    if (arg)
    printf("testdest: Unknown option \"%s\".\n", arg);
    puts("Usage:");
    puts("  ./testdest name [operation ...]");
    puts("  ./testdest ipp://... [operation ...]");
    puts("  ./testdest ipps://... [operation ...]");
    puts("  ./testdest --enum [grayscale] [color] [duplex] [staple] [small]\n"
         "                    [medium] [large]");
    puts("");
    puts("Operations:");
    puts("  conflicts options");
    puts("  default option");
    puts("  localize option [value]");
    puts("  media [borderless] [duplex] [exact] [ready] [name or size]");
    puts("  print filename [options]");
    puts("  supported [option [value]]");
    exit(arg != NULL);
}