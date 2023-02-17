//Формат листов А3
//Расширения файлов, доступных для печати: pdf, jpeg
#include <stdio.h>
#include <cups/cups.h>

//структура для хранения информации с пунктом назначения печати
typedef struct {
    cups_dest_t *dest;              //указатель на принтер по умолчанию
    http_t *http;                   //указатель на соединение с планировщиком или принтером по умолчанию
    cups_dinfo_t *info;             //информация о возможностях места назначения
} def_dest_s;

static int	enum_cb (void *user_data, unsigned flags, cups_dest_t *dest);
static void usage (const char *arg) __attribute__((noreturn));

int main(int argc, char *argv[]) {

    def_dest_s main_dest = {.dest = NULL};   //экзамепляр принтера по умолчанию

    if (argc < 2) {
        usage(NULL);
    }

    if (!strcmp(argv[1], "--enum")) {
        int                 i;              //переменная для for
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
            else {
                usage(argv[i]);
            } 
        }
        cupsEnumDests(CUPS_DEST_FLAGS_NONE, 5000, NULL, type, mask, enum_cb, NULL);

        return 0;
    }   //конец обработки опции --enum
    else if (!strcmp(argv[1], "--set")) {   //опция по настройке .config файла
        if (!strcmp(argv[2], "--config")) {    //изменение принтера по умолчанию
            if (cupsGetNamedDest(CUPS_HTTP_DEFAULT, argv[2], NULL)) {
                FILE *config_f;

                if (config_f = fopen(".config", "a") == NULL) {
                    printf ("Could not open the file \".config\"\n");
                    return 1;
                }
            }
        }
    }
    return 0;
}

/*
 * 'enum_cb()'          - print the results from the enumeration of destinations.
 *              OUTPUT
 * int - 1 to continue.
 *              INPUT
 * 'void *user_data'    - user data (unused),
 * 'unsigned flaga'     - flags,
 * 'cups_dest_t *dest'  - destination.
 */
static int enum_cb (void *user_data, unsigned flags, cups_dest_t *dest) {
    int	i;				/* Looping var */

    (void)user_data;
    (void)flags;

    if (dest->instance)
        printf("%s/%s:\n", dest->name, dest->instance);
    else
        printf("%s:\n", dest->name);
    
    for (i = 0; i < dest->num_options; i ++)
        printf("    %s=\"%s\"\n", dest->options[i].name, dest->options[i].value);
    return 1;
}