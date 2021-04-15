#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/shm.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/mman.h> //mmap head file
#include <linux/tcp.h>
#include <linux/types.h>
#include <asm/byteorder.h>
#include <linux/ip.h>
#include "network_client.h"

#define MIN(a,b) (a>b?b:a)

#define  CLIENT_BUFF_SIZE 2048


static Net_Circle_Buff_t net_buff;

static int read_data(Net_Circle_Buff_t *net_ptr,int len);
static void network_client_connect(void);

void network_init(void )
{
    network_client_connect();
    net_buff.local_buff_info.head_index_offset = 0;
    net_buff.local_buff_info.tail_index_offset = 0;
    net_buff.local_buff_info.element_length = CLIENT_BUFF_SIZE;
    net_buff.local_buff_info.buff_length = MAX_SIZE;
    net_buff.local_buff_info.semaphore = 1;

    pthread_mutex_init(&net_buff.mutex, NULL);
    pthread_cond_init(&net_buff.notfull, NULL);
    pthread_cond_init(&net_buff.notempty, NULL);
}
void network_client_connect(void)
{
    net_buff.fd = socket(AF_INET, SOCK_STREAM, 0);
    if( net_buff.fd == -1)
    {
        perror("socket failed");
        exit(0);
    }

    // 搞一个IPv4的地址结构体，并赋值为服务端的地址
    struct sockaddr_in srvaddr;
    bzero(&(srvaddr), sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.56.6", (void *)&(srvaddr.sin_addr));
    srvaddr.sin_port = htons(atoi("8111"));

    // 链接对方
    connect(net_buff.fd, (struct sockaddr *)&(srvaddr), sizeof(srvaddr));

    net_buff.fp = fdopen(net_buff.fd,"r+");
    if (net_buff.fp == NULL){
        perror("fopen()");
        exit(1);
    }
}
void network_close(void *p)
{
    pthread_mutex_destroy(&net_buff.mutex);
    pthread_cond_destroy(&net_buff.notfull);
    pthread_cond_destroy(&net_buff.notempty);
    close(net_buff.fd);
}

void network_get(Net_Circle_Buff_t **net_ptr)
{
    *net_ptr =  &net_buff;
}


void *network_producer( void *arg )
{
    static int count = 0;
    pthread_cleanup_push(network_close,NULL);
    while(1)
    {
        struct tcp_info info;
        int len=sizeof(info);
        getsockopt(net_buff.fd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
        //printf("info.tcpi_state:%d\n",info.tcpi_state);
        if(info.tcpi_state==1)
        {
            pthread_mutex_lock(&net_buff.mutex);  // 为保证条件变量不会因为多线程混乱，所以先加锁
            while(is_buff_full(&net_buff.local_buff_info))
            {
                printf("buff full!\n");
                pthread_cond_wait( &net_buff.notfull, &net_buff.mutex);
            }
            read_data(&net_buff,CLIENT_BUFF_SIZE);
            pthread_mutex_unlock(&net_buff.mutex);

            pthread_cond_signal(&net_buff.notempty);
            printf("send buff not empty signal %d\n",count++);
            pthread_yield();

        }else{
            printf("disconnect!\n");
            // how to reconnect
        }
    }
    pthread_cleanup_pop(1);
    pthread_exit(NULL);
}


int read_data(Net_Circle_Buff_t *net_ptr,int len)
{
    int total_len = len;
    int read_len = 0;
    int rtn_len = 0;
    char buffer[CLIENT_BUFF_SIZE] = {0};
    while(total_len)
    {
        read_len = MIN(total_len, CLIENT_BUFF_SIZE);
        rtn_len = fread(buffer, sizeof(char), read_len, net_ptr->fp);
        if(rtn_len < read_len)  /* 读到数据小于预期 */
        {
            if(ferror(net_ptr->fp))
            {
                if(errno == EINTR) /* 信号使读操作中断 */
                {
                    /* 不做处理继续往下走 */;
                }
                else if(errno == EAGAIN || errno == EWOULDBLOCK) /* 发生了超时 */
                {
                    total_len -= rtn_len;
                    break;
                }
                else    /* 其他错误 */
                {
                    exit(1);
                }
            }
            else    /* 读到文件尾 */
            {
                total_len -= rtn_len;
                break;
            }
        }
        total_len -= rtn_len;
    }
    if(total_len != 0)
    {
        return -1;
    }else{
        printf("push! cmd :%d,\n", *((int *)buffer));
        push_circle_buff_item(&net_ptr->local_buff_info,net_ptr->local_buff,buffer);
        return 0;
    }
}

