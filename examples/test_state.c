/*
 * Program to test with getting state of destination
 */


#include <cups/cups.h>
#include <stdio.h>

int main (int argc, char argv[]) 
{
    cups_dest_t        *dest; //pointer to destination
    const char *value;

    dest = cupsGetDestWithURI(NULL, argv[1]);
    if (dest == NULL)
    {
        printf ("Unknown printer\n");
        return 1;
    }

    value = cupsGetOption ("printer-info", dest->num_options, dest->options);
    printf("%s (%s)\n", dest->name, value ? value : "no description");
    value = cupsGetOption ("printer-state-reasons", dest->num_options, dest->options);
    printf("%s (%s)\n", dest->name, value);

    return 0;
}