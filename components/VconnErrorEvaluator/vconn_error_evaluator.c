/*
 * Copyright 2021, Olof Holmberg
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <string.h>
#include <camkes.h>

int run(void)
{
    memset(dest, '\0', 4096);

    while (1) {
        ready_vee_wait();
        
        int vconn_error;
        memcpy(&vconn_error, src, sizeof(vconn_error));
        
        int retval = vconn_error == EAGAIN ? 0 : vconn_error;
        memcpy(dest, &retval, sizeof(retval)); 
        
        done_vee_emit_underlying();
    }

    return 0;
}
