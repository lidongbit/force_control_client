#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>           //mmap head file
#include <unistd.h>
#include <sys/shm.h>

#include "force_interface.h"
#include "mmap_comm.h"          //comm with force core
#include "shmem_client.h"       //comm with server
#include "controller.h"


static void cleanup_handler(void);

static void cleanup_handler(void)
{
    shmem_close();
    mmap_close();
}


int main()
{
    printf("force client init!\n");
    mmap_init();
    shmem_init();
    Force_Servo_Comm_Init(); 
    atexit(cleanup_handler);

    printf("force client init ok!\n");

    pthread_t tid_comm,tid_show;
    pthread_create(&tid_comm,NULL,controller_msg_process,NULL);
    //pthread_create(&tid_show,NULL,controller_display_result,NULL);

    pthread_join(tid_comm,NULL);
    //pthread_join(tid_show,NULL);
    return 0;
}
