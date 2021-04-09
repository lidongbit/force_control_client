#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "controller.h"
#include "force_interface.h"
#include "string.h"
typedef buffer_info_t SERVO_COMM_RINGS_BUFF_STRUCT;
static int line_count = 0;
static SERVO_COMM_RINGS_BUFF_STRUCT *local_buff;

static void controller_parase_cmd(char *buff, int len)
{
    ServoCoreProcessCall_t *cmd_para = (ServoCoreProcessCall_t*)buff;
    char *ptr = buff+sizeof(ServoCoreProcessCall_t);
    if(sizeof(ServoCoreProcessCall_t)>len
            ||(sizeof(ServoCoreProcessCall_t)+sizeof(PARA_READ_INFO_t))>len
            ||(sizeof(ServoCoreProcessCall_t)+sizeof(FORCE_INSTRUCTION_INFO_t))>len)
    {
        printf("parase para err!");
        return;
    }

    printf("reveive cmd: %d\n",cmd_para->cmd);
    switch(cmd_para->cmd)
    {
    case CMD_ENABLE:
        Force_Servo_Comm_Cmd_Enable();
        break;
    case CMD_WORKING:
        Force_Servo_Comm_Cmd_Enable();
        break;
    case CMD_IDLE:
        Force_Servo_Comm_Cmd_Idle();
        break;
    case CMD_ABORT:  //discard instruction
        Force_Servo_Comm_Cmd_Abort();
        break;
    case CMD_CONTINUE:
        Force_Servo_Comm_Cmd_Continue();
        break;
    case CMD_DISABLE:
        Force_Servo_Comm_Cmd_Disable();
        break;
    case CMD_RESET_FAULT:
        Force_Servo_Comm_Cmd_ResetFault();
        break;
    case CMD_SWITCH_MODE:
        Force_Servo_Comm_Cmd_Mode(cmd_para->param1,cmd_para->param2);
        break;
    case CMD_DIGNOSE_CONFIG:
        Force_Servo_Comm_Cmd_DignoseCfg((FORCE_DIGNOSE_VAR_e*)ptr);
        break;      
    case CMD_SET_PARA:
        Force_Servo_Comm_Set_Para((PARA_READ_INFO_t*)ptr);
        break;
    case CMD_SET_INS:
        Force_Servo_Comm_Set_Instruct((FORCE_INSTRUCTION_INFO_t*)ptr,512);
        break;
    case CMD_SHAKE_REQ:
        Force_Servo_Comm_ShakeHand_Request();
        break;
    case CMD_SHAKE_BIND:
        Force_Servo_Comm_ShakeHand_Bind();
        break;
    case CMD_HEART_INC:
        Force_Servo_Comm_ServoHeart_Inc();
        break;
    default:
        break;
    }
}

static void controller_display_state(int state)
{
    switch(state)
    {
    case SM_BOOT:
        printf("state: SM_BOOT\n");
        break;
    case SM_INIT:
        printf("state: SM_INIT\n");
        break;
    case SM_IDLE:
        printf("state: SM_IDLE\n");
        break;
    case SM_WORKING:
        printf("state: SM_WORKING\n");
        break;
    case SM_ABORT:
        printf("state: SM_ABORT\n");
        break;
    case SM_ERROR:
        printf("state: SM_ERROR\n");
        break;
    default:
        printf("state: \n");
        break;
    }
}


static void controller_display_mode(int mode)
{
    switch(mode)
    {
    case NO_MODE:
        printf("mode: NO_SUB_MODE\n");
        break;
    case IMPEDANCE_MODE:
        printf("mode: IMPEDANCE_MODE\n");
        break;
    case ADMITTANCE_MODE:
        printf("mode: ADMITTANCE_MODE\n");
        break;
    default:
         printf("mode: \n");
        break;
    }
}

static void controller_display_submode(int mode)
{
    switch(mode)
    {
    case NO_SUB_MODE:
        printf("submode: NO_SUB_MODE\n");
        break;
    case TEACH:
        printf("submode: TEACH\n");
        break;
    case LEARN:
        printf("submode: LEARN\n");
        break;
    default:
         printf("submode: \n");
        break;
    }
}

void *controller_display_result(void *p)
{
    //pthread_detach(pthread_self());
    FORCE_BACK_INFO_t res;
    int heart = Force_Servo_Comm_Get_ForceHeart();
    int ret = Force_Servo_Comm_Get_Result(&res);
    if(ret==-1)
    {
        pthread_yield();
    }
    printf("force core heart: %d\n",heart);
    printf("err_code: %X\n",res.err_code);
    printf("err_history[4]: %X %X %X %X\n",res.err_history[0],res.err_history[1],res.err_history[2],res.err_history[3]);
    controller_display_state(res.state);
    controller_display_mode(res.mode);
    controller_display_submode(res.sub_mode);
    line_count += 6;
    usleep(200);
    printf("\r\033[%dA",line_count);
}

#define BUFF_SIZE 1024
void *controller_msg_process(void *p)
{
    //pthread_detach(pthread_self());
    char buff[BUFF_SIZE] = {0};
    shmem_get(&local_buff);
    int res = 0;
    while(1)
    {
        res = 0;
        while(is_buff_empty(local_buff))
        {
            //printf("controller idle!\n");
            pthread_yield();
        }
        res = pull_circle_buff_item(local_buff,buff);

        printf("receive cmd:%d\n",*((int*)buff));
        if(res<0)
        {
            printf("pull_circle_buff_item error!\n");
        }
        //controller_parase_cmd(buff,BUFF_SIZE);
        bzero(buff, BUFF_SIZE);
    }

    pthread_exit(0);
}
