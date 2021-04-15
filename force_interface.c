#include <string.h>
#include <stdio.h>
#include "force_interface.h"
#include "mmap_comm.h"

static SERVO_COMM_RINGS_BUFF_STRUCT *circle_buff_cmd_info;
static char *circle_buff_cmd;

static SERVO_COMM_RINGS_BUFF_STRUCT *circle_buff_res_info;
static char *circle_buff_res;

static int *para_update_flag;
static PARA_READ_INFO_t *para_info;

static MMAP_COMM_PTR_t *mmap_ptr;

static  void Force_Servo_Comm_Set_Cmd(ServoCoreProcessCall_t *app);

void Force_Servo_Comm_Init(void)
{
    mmap_get_comm_ptr(&mmap_ptr);

    para_update_flag = (int*)mmap_ptr->para_ptr;

    para_info = (PARA_READ_INFO_t*)(mmap_ptr->para_ptr+8);

    circle_buff_cmd_info = (SERVO_COMM_RINGS_BUFF_STRUCT*)mmap_ptr->ins_ctrl_ptr;
    circle_buff_cmd = (char *)mmap_ptr->ins_msg_ptr;

    circle_buff_cmd_info->element_length = sizeof(FORCE_INSTRUCTION_INFO_t);
    circle_buff_cmd_info->buff_length = MESAGE_BUFF_LENGTH;
    circle_buff_cmd_info->semaphore = 1;
    circle_buff_cmd_info->tail_index_offset = 0;
    circle_buff_cmd_info->head_index_offset = 0;
    circle_buff_cmd_info->safety_resevd = 2*circle_buff_cmd_info->element_length;
    circle_buff_cmd_info->write_lock = 0;

    circle_buff_res_info = (SERVO_COMM_RINGS_BUFF_STRUCT*)mmap_ptr->res_ctrl_ptr;
    circle_buff_res_info->element_length = sizeof(FORCE_BACK_INFO_t);
    circle_buff_res_info->buff_length = MESAGE_BUFF_LENGTH;
    circle_buff_res = (char*)mmap_ptr->res_msg_ptr;
    //printf("comm init ok!\n");
}

int Force_Servo_Comm_Get_Para_UpdateFlag(void)
{
    return *(para_update_flag);
}

void Force_Servo_Comm_Set_Para_UpdateFlag(int val)
{
    *(para_update_flag) = val;
}


void Force_Servo_Comm_ShakeHand_Request(void)
{
    *((int *)mmap_ptr->comm_ptr) = MAGIC_REQ_DATA;
    //printf("shake hand!\n");
}

void Force_Servo_Comm_ShakeHand_Bind(void)
{
    while(*((int *)mmap_ptr->comm_ptr)!=MAGIC_BIND_DATA)
        ;
    *((int *)mmap_ptr->comm_ptr) = MAGIC_CONNECT_DATA;
}

void Force_Servo_Comm_ServoHeart_Inc(void)
{
    *((int *)(mmap_ptr->comm_ptr+4)) = *((int *)(mmap_ptr->comm_ptr+4)) + 1;
}

int Force_Servo_Comm_Get_ForceHeart(void)
{
    return *((int *)(mmap_ptr->comm_ptr+8));
}
int Force_Servo_Comm_Get_ServoHeart(void)
{
    return *((int *)(mmap_ptr->comm_ptr+4));
}
static void Force_Servo_Comm_Set_Cmd(ServoCoreProcessCall_t *app)
{
    memcpy(mmap_ptr->cmd_ptr,app,sizeof(ServoCoreProcessCall_t));
}

void Force_Servo_Comm_Cmd_Enable(void)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_ENABLE;
    Force_Servo_Comm_Set_Cmd(&app);
}

void Force_Servo_Comm_Cmd_Working(void)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_WORKING;
    Force_Servo_Comm_Set_Cmd(&app);
}

void Force_Servo_Comm_Cmd_Idle(void)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_IDLE;
    Force_Servo_Comm_Set_Cmd(&app);
}

void Force_Servo_Comm_Cmd_Abort(void)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_ABORT;
    Force_Servo_Comm_Set_Cmd(&app);
}

void Force_Servo_Comm_Cmd_Continue(void)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_CONTINUE;
    Force_Servo_Comm_Set_Cmd(&app);
}

void Force_Servo_Comm_Cmd_Disable(void)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_DISABLE;
    Force_Servo_Comm_Set_Cmd(&app);
}

void Force_Servo_Comm_Cmd_ResetFault(void)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_RESET_FAULT;
    Force_Servo_Comm_Set_Cmd(&app);
}

void Force_Servo_Comm_Cmd_Mode(int mode, int submode)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_SWITCH_MODE;
    app.param1 = mode;
    app.param2 = submode;
    Force_Servo_Comm_Set_Cmd(&app);
}

void Force_Servo_Comm_Cmd_DignoseCfg(FORCE_DIGNOSE_VAR_e *ptr)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_SWITCH_MODE;

    app.param1 = ptr[0] | (ptr[1]<<16);
    app.param2 = ptr[2] | (ptr[3]<<16);
    app.param3 = ptr[4] | (ptr[5]<<16);
    app.param4 = ptr[6] | (ptr[7]<<16);

    Force_Servo_Comm_Set_Cmd(&app);
}

void Force_Servo_Comm_Set_Para(PARA_READ_INFO_t *para_ptr)
{
    memcpy(mmap_ptr->para_ptr,para_ptr,sizeof(PARA_READ_INFO_t));
    Force_Servo_Comm_Set_Para_UpdateFlag(1);
}

int Force_Servo_Comm_Set_Instruct(FORCE_INSTRUCTION_INFO_t *push_ptr,int32_t length)
{
    int res = 0;
    if(push_ptr==NULL || length<0)
    {
        return -1;
    }

    res = push_circle_buff_item(circle_buff_cmd_info, circle_buff_cmd, push_ptr);
    return res;
}

int Force_Servo_Comm_Get_Result(FORCE_BACK_INFO_t *pull_ptr)
{
    int res = 0;
    res = pull_circle_buff_item(circle_buff_res_info, circle_buff_res, pull_ptr);
    return res;
}
