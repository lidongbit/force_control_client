#ifndef _SHMEM_COMM_H__
#define _SHMEM_COMM_H__
#include "buffer_manager.h"
typedef buffer_info_t DEBUG_RINGS_BUFF_STRUCT;
extern void shmem_init(void);
extern void shmem_close(void);
extern void shmem_get(DEBUG_RINGS_BUFF_STRUCT **ptr,char **buff);
#endif
