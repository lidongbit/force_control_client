#ifndef __MMAP_COMM_H__
#define __MMAP_COMM_H__

typedef struct
{
    void  *comm_ptr;
    void  *para_ptr;
    void  *cmd_ptr;
    void  *cmd_bak_ptr;
    void  *ins_ctrl_ptr;
    void  *ins_msg_ptr;
    void  *res_ctrl_ptr;
    void  *res_msg_ptr;
}MMAP_COMM_PTR_t;

extern void mmap_init(void);
extern void mmap_get_comm_ptr(MMAP_COMM_PTR_t **ptr);
extern void mmap_close(void);
#endif
