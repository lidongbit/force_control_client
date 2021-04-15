#ifndef __NETWORK_CLIENT_H__
#define __NETWORK_CLIENT_H__
#include "buffer_manager.h"
#include <pthread.h>
#include <stdio.h>

#define  MAX_SIZE 10*4096

typedef struct
{
    int fd;
    FILE *fp;

    buffer_info_t local_buff_info;
    char local_buff[MAX_SIZE];

    pthread_mutex_t mutex;
    pthread_cond_t notfull;  //缓冲区不满条件变量
    pthread_cond_t notempty; //缓冲区不空条件变量
}Net_Circle_Buff_t;
typedef buffer_info_t DEBUG_RINGS_BUFF_STRUCT;
extern void network_init(void);
extern void network_close(void*);
extern void network_get(Net_Circle_Buff_t **net_ptr);
extern void *network_producer(void *arg);
#endif
