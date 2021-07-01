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
    
    int nr_execution = 0;

    while (1) {
        ready_init_wait();
        
        nr_execution++;
        
        printf("--- Execution nr: %d ---", nr_execution);
        
        int vconn_error;
        memcpy(&vconn_error, src, sizeof(vconn_error));
        printf("Obtained error code: %d\n", vconn_error);
        
        int retval = vconn_error == EAGAIN ? 0 : vconn_error;
        memcpy(dest, &retval, sizeof(retval)); 
        
        printf("seL4 execution done with vconn_error: %d\n", retval);
        done_init_emit_underlying();
    }

    return 0;
}
