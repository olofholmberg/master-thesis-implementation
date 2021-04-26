/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#include <string.h>
#include <camkes.h>

int run(void)
{
    memset(dest, '\0', 4096);
    char str[200];
    sprintf(str, "This is a crossvm dataport test string\n");
    strcpy(dest, str);
    
    int event_nr = 0;

    while (1) {
        ready_init_wait();
        printf("Got an event\n");
        event_nr++;
        memset(str, 0, sizeof str);
        strcpy(str, src);
        printf("Printing from src: \"%s\"", str);
        memset(src, '\0', 4096);
        memset(dest, '\0', 4096);
        memset(str, 0, sizeof str);
        sprintf(str, "This is a crossvm dataport test event string to dest: %d\n", event_nr);
        strcpy(dest, str);
        done_init_emit_underlying();
    }

    return 0;
}
