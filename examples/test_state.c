/*
 * Program to test with getting state of destination
 */


#include <cups/cups.h>
#include <stdio.h>
#include <string.h>

int main (int argc, char *argv[]) 
{
    cups_dest_t                     *dest; //pointer to destination
    const char                     *value;
    int         num_printer_state_reasons = 0,
                        num_printer_state = 0;

    if (!strncmp(argv[1], "ipp://", 6) || !strncmp(argv[1], "ipps://", 7))
        dest = cupsGetDestWithURI(NULL, argv[1]);
    else
        dest = cupsGetNamedDest(CUPS_HTTP_DEFAULT, argv[1], NULL);
    if (dest == NULL)
    {
        printf ("Unknown printer\n");
        return 1;
    }

    printf("%s\n", dest->name);
    value = cupsGetOption("printer-state", dest->num_options, dest->options);
    printf("printer-state = (%s)\n", value);
    value = cupsGetOption("printer-state-reasons", dest->num_options, dest->options);
    printf("printer-state-reasons = (%s)\n", value);

    return 0;
}