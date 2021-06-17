/*
 * Copyright 2020, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>

void block_event(int fd) {
	int val;
	/* Blocking read */
	int result = read(fd, &val, sizeof(val));
	if (result < 0) {
	    printf("Error: %s\n", strerror(errno));
	} 

}

void emit_event(char* emit) {
	emit[0] = 1;
}

void memcpy_byte(void *dst, void *src, size_t size) {
    char *dst_c = dst;
    char *src_c = src;
    for (int i = 0; i < size; i++) {
        *dst_c = *src_c;
        dst_c++;
        src_c++;
    }
}

int main(int argc, char *argv[])
{
    
    char *uio_one_string = "This is a uio1 string\n\0";

    /* Setup connection with native component via uio0 device file */
    char *dataport_name = "/dev/uio0";
    int length = 4096;
    
    char *dataport1_name = "/dev/uio1";

    int fd = open(dataport_name, O_RDWR);
    assert(fd >= 0);
    
    int fd1 = open(dataport1_name, O_RDWR);
    assert(fd >= 0);

    void *dataport;
    if ((dataport = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 1 * getpagesize())) == (void *) -1) {
        printf("mmap dataport0 failed\n");
        close(fd);
    }
    
    void *dataport1;
    if ((dataport1 = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 1 * getpagesize())) == (void *) -1) {
        printf("mmap dataport1 failed\n");
        close(fd);
    }

    char *emit;
    if ((emit = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 * getpagesize())) == (void *) -1) {
        printf("mmap emit failed\n");
        close(fd);
    }
    
    printf("CVC: Reading uio0\n");
    for (char *chr = dataport; *chr != 0; chr ++) {
        putchar(*chr);
    }
    fflush(stdout);

    printf("CVC: Writing uio1\n");
    size_t uio_one = uio_one_string ? strnlen(uio_one_string, 4095) + 1 : 0;
    strncpy(dataport1, uio_one_string, uio_one);
    memcpy_byte(dataport1, uio_one_string, uio_one);

    printf("CVC: Reading uio0\n");
    for (char *chr = dataport; *chr != 0; chr ++) {
        putchar(*chr);
    }
    fflush(stdout);
    
    printf("CVC: Emitting uio0\n");
    emit_event(emit);
    
    printf("CVC: Waiting for done_init_emit_underlying in crossvm_init\n");
    block_event(fd);

    printf("Finished crossvm test script\n");
    
    munmap(dataport, length);
    munmap(emit, length);
    close(fd);

    return 0;
}
