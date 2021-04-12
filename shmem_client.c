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
#include "shmem_client.h"

#define  CLIENT_BUFF_SIZE 2048
#define  MAX_SIZE 4096



static DEBUG_RINGS_BUFF_STRUCT *local_buff_info;
static char *local_buff;
static void *shm_ctrl;
static void *shm_msg;
static int shmid_ctrl,shmid_msg;  //创建共享内存
void shmem_init(void )
{

    shmid_ctrl = shmget((key_t)1234, sizeof(DEBUG_RINGS_BUFF_STRUCT), 0666|IPC_CREAT);
    shmid_msg = shmget((key_t)2345, MAX_SIZE, 0666|IPC_CREAT);
    if( shmid_msg == -1 || shmid_ctrl == -1)
    {
       fprintf(stderr, "shmget failed\n");
       exit(EXIT_FAILURE);
    }

    //将共享内存连接到当前进程的地址空间
    shm_ctrl = shmat(shmid_ctrl, (void*)0, 0);
    shm_msg = shmat(shmid_msg,(void*)0,0);

    if(shm_ctrl == (void*)-1 || shm_msg == (void*)-1)
    {
       fprintf(stderr, "shmat failed\n");
       exit(EXIT_FAILURE);
    }
   // printf("Memory attached at 0x%X,0x%X\n", (int)shm_ctrl,(int)shm_msg);    //设置共享内存
    local_buff_info = (DEBUG_RINGS_BUFF_STRUCT*)shm_ctrl;
    local_buff_info->head_index_offset = 0;
    local_buff_info->tail_index_offset = 0;
    local_buff_info->element_length = CLIENT_BUFF_SIZE;
    local_buff_info->buff_length = MAX_SIZE;
    local_buff_info->semaphore = 1;
    local_buff = (char*)shm_msg;
    //printf("shmem init ok!\n");
}

void shmem_close(void)
{
    if(shmdt(shm_ctrl) == -1 || shmdt(shm_msg) == -1)
    {
       fprintf(stderr, "shmdt failed\n");
       exit(EXIT_FAILURE);
    }
}

void shmem_get(DEBUG_RINGS_BUFF_STRUCT **ptr,char **buff)
{
    *ptr = local_buff_info;
    *buff  = local_buff;
}
