#ifndef __BUFFER_MANAGER_H__
#define __BUFFER_MANAGER_H__
#include "stdint.h"
typedef struct
{
        int32_t tail_index_offset;
        int32_t head_index_offset;
        int32_t element_length;
        int32_t buff_length;
        int32_t safety_resevd;
        int32_t semaphore;//初始化需要置位1
        int32_t write_lock;
        char * buff;
}buffer_info_t;

extern int32_t push_circle_buff_item(buffer_info_t *circle_buff_info, void *push_ptr, int32_t length);
extern int32_t push_circle_buff_bundle(buffer_info_t *circle_buff_info, buffer_info_t *local_buff);

extern int32_t pull_circle_buff_item(buffer_info_t *circle_buff_info, void *pull_ptr);
extern int32_t pull_circle_buff_bundle(buffer_info_t *circle_buff_info, buffer_info_t *local_buff);

extern int32_t is_buff_full(buffer_info_t*);
extern int32_t is_buff_empty(buffer_info_t*);
#endif
