#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h> //mmap head file
#include <unistd.h>
#include <sys/shm.h>
#include "force_interface.h"
#include "mmap_comm.h"

static MMAP_COMM_PTR_t mmap_addr;

void mmap_init(void)
{
    int fd = open("/dev/mem",O_RDWR|O_SYNC);
    if(fd<0)
    {
        printf("open /dev/mem failed!\n");
        exit(0);
    }
    mmap_addr.comm_ptr = mmap( NULL,1024,PROT_READ|PROT_WRITE,MAP_SHARED,fd,SERVO_COMM_ADDR);
    if(mmap_addr.comm_ptr == (void *)-1)
    {
        printf("mmap 1 failure !\n");
        exit(0);
    }
    mmap_addr.cmd_ptr = mmap( NULL,1024,PROT_READ|PROT_WRITE,MAP_SHARED,fd,SERVO_CMD_ADDR);
    if(mmap_addr.cmd_ptr == (void *)-1)
    {
        printf("mmap 2 failure!\n");
        munmap(mmap_addr.comm_ptr,1024);
        exit(0);
    }
    mmap_addr.para_ptr = mmap( NULL,1024,PROT_READ|PROT_WRITE,MAP_SHARED,fd,SERVO_PARA_UPDATE_FLAG);
    if(mmap_addr.para_ptr == (void *)-1)
    {
        printf("mmap 3 failure!\n");
        munmap(mmap_addr.comm_ptr,1024);
        munmap(mmap_addr.cmd_ptr,1024);
        exit(0);
    }

    mmap_addr.ins_ctrl_ptr = mmap( NULL,1024,PROT_READ|PROT_WRITE,MAP_SHARED,fd,SERVO_INSTRUCT_INFO_ADDR);
    if(mmap_addr.ins_ctrl_ptr == (void *)-1)
    {
        printf("mmap 4 failure!\n");
        munmap(mmap_addr.comm_ptr,1024);
        munmap(mmap_addr.cmd_ptr,1024);
        munmap(mmap_addr.para_ptr,1024);
        exit(0);
    }
    mmap_addr.ins_msg_ptr = mmap( NULL, 4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,SERVO_INSTRUCT_ADDR);
    if(mmap_addr.ins_msg_ptr == (void *)-1)
    {
        printf("mmap 5 failure!\n");
        munmap(mmap_addr.comm_ptr,1024);
        munmap(mmap_addr.cmd_ptr,1024);
        munmap(mmap_addr.para_ptr,1024);
        munmap(mmap_addr.ins_ctrl_ptr,1024);
        exit(0);
    }
    mmap_addr.res_ctrl_ptr = mmap( NULL,1024,PROT_READ|PROT_WRITE,MAP_SHARED,fd,SERVO_RES_INFO_ADDR);
    if(mmap_addr.res_ctrl_ptr == (void *)-1)
    {
        printf("mmap 6 failure!\n");
        munmap(mmap_addr.comm_ptr,1024);
        munmap(mmap_addr.cmd_ptr,1024);
        munmap(mmap_addr.para_ptr,1024);
        munmap(mmap_addr.ins_ctrl_ptr,1024);
        munmap(mmap_addr.ins_msg_ptr,4096);
        exit(0);
    }
    mmap_addr.res_msg_ptr =  mmap( NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,SERVO_RES_ADDR);
    if(mmap_addr.res_msg_ptr == (void *)-1)
    {
        printf("mmap 7 failure!\n");
        munmap(mmap_addr.comm_ptr,1024);
        munmap(mmap_addr.cmd_ptr,1024);
        munmap(mmap_addr.para_ptr,1024);
        munmap(mmap_addr.ins_ctrl_ptr,1024);
        munmap(mmap_addr.ins_msg_ptr,4096);
        munmap(mmap_addr.res_ctrl_ptr,1024);
        exit(0);
    }

}

void mmap_get_comm_ptr(MMAP_COMM_PTR_t **ptr)
{
    *ptr = &mmap_addr;
}

void mmap_close(void)
{
    munmap( mmap_addr.comm_ptr, 1024);
    munmap( mmap_addr.cmd_ptr, 1024);
    munmap( mmap_addr.para_ptr, 1024);
    munmap( mmap_addr.ins_ctrl_ptr, 1024);
    munmap( mmap_addr.ins_msg_ptr, 4096);
    munmap( mmap_addr.res_ctrl_ptr, 1024);
    munmap( mmap_addr.res_msg_ptr, 4096);

    printf( "umap ok \n" );
}
