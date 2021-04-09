#ifndef __FORCE_SERVO_H__
#define __FORCE_SERVO_H__
#include "buffer_manager.h"

/*communication shake*/
#define SERVO_COMM_ADDR  (0x3b000000)
#define COMM_SHAKE_HANDE_DATA (*((volatile unsigned int*)0x3b000000))
#define SERVO_CORE_HEART (*((volatile unsigned int*)0x3b000004))
#define FORCE_CORE_HEART (*((volatile unsigned int*)0x3b000008))

/*command interface*/
#define SERVO_CMD_ADDR  (0x3b001000)            //
#define SERVO_CMD_BACK_ADDR (0x3b001020)

/*parameters interface*/
#define SERVO_PARA_UPDATE_FLAG (0x3b002000)
#define SERVO_PARA_ADDR  (0x3b002008)


/*instruction interface*/
#define SERVO_INSTRUCT_INFO_ADDR  (0x3b003000)
#define SERVO_INSTRUCT_ADDR  (0x3b004000)


/*feedback interface*/
#define SERVO_RES_INFO_ADDR (0x3b005000)
#define SERVO_RES_ADDR (0x3b006000)


/*const value*/
#define MAGIC_REQ_DATA 0xaa55aa55
#define MAGIC_BIND_DATA 0x55aa55aa
#define MAGIC_CONNECT_DATA 0xa5a5a5a5

#define MESAGE_BUFF_LENGTH 4096


typedef struct
{
    double x;
    double y;
    double z;
}Vector3;

typedef struct
{
    double a1;
    double a2;
    double a3;
    double a4;
    double a5;
    double a6;
}Vector6;

typedef struct
{
    double d;
    double a;
    double alpha;
    double offset;
}DH;

typedef struct
{
    int arm;    // 1: right arm, -1:left arm
    int elbow;  // 1: elbow above wrist, -1:elbow below wrist
    int wrist;  // 1: wrist down, -1: wrist up
    int flip;   // 0: not flip wrist, 1: flip wrist
}Posture;

typedef struct
{
    double matrix_[3][3];
}Matrix33;

typedef Matrix33 RotationMatrix;
typedef Vector3 Point;

typedef struct
{
    RotationMatrix rotation_matrix_;
    Point trans_vector_;
}TransMatrix;

typedef enum
{
    NO_MODULE = 0x00,
    PARAM_MOUDLE = 0x01,
    ADNITTANCE_MOUDLE = 0x02,//dao na
    IMPEDANCE_MOUDLE = 0x03,//zu kang
    INSTRUCTION_MOUDLE = 0x04,//zhiling
    KINEMATIC_MOUDLE = 0x05,//forward kinematic
    KINEMATIC_INV_MOUDLE = 0x06,//inverse kinematic
    SERVO_COMM_MOUDLE = 0x07,//communication module
    DIFF_MOTION_MOUDLE = 0x08,
    MODULE_MAX = DIFF_MOTION_MOUDLE +1,
}FORCE_MODULE_e;

typedef enum
{
    HEART_ERR = 0x01,/*communication error*/
    CMD_ERR = 0x02,/*communication error*/
    INSTRUCTION_LOSE_ERR = 0x03,/*instruction buff empty*/
    INSTRUCTION_OVER_LIMIT_ERR = 0x04,/*instruction invalid*/
    RESULT_PUSH_ERR = 0x05,/*result buff full*/
    DIVED_ZERO = 0x06,
    KINEMATIC_CALC_ERR = 0x07,
    DIFF_CALC_ERR = 0x08,
    KINEMATIC_INV_CALC_ERR = 0x09,
}FORCE_ERR_CODE_e;

typedef enum
{
    POS_POSEULAR_GIVEN,
    THETA_BACK,
    THETA_BACK_TO_POS_EULAR,
    THETA_BACK_TO_POS_QUANT,
    OUT_POS_COMP,
    IN_FORCE_GIVEN_BASE,
    IN_FORCE_BACK_TOOL,
    IN_FORCE_BACK_BASE,
    FORCE_ERR_BASE,
    OUT_THETA,
    PID_ERR,
    PID_OUT,
    VAR_MAX = PID_OUT + 1,
}FORCE_DIGNOSE_VAR_e;
typedef enum FORCE_CORE_STATE
{
    SM_BOOT = 0,
    SM_INIT = 1,
    SM_IDLE = 2,
    SM_WORKING = 3,
    SM_ABORT = 4,
    SM_ERROR = 5,
}FORCE_CORE_STATE_e;

/******************para*****************/
typedef struct{
    double kp;
    double ki;
    double kd;
}FORCE_PARA_PID_t;

typedef struct FORCE_PARA_IMPEDANCE
{
    Vector6 Bd;//Expected damping matrix 阻尼
    Vector6 Kd;//expected impedance matrix 阻抗
    Vector6 Md;//expected stiffness matrix 刚度
}FORCE_PARA_IMPEDANCE_t;//阻抗控制参数

typedef struct FORCE_PARA_ROBOTIC
{
   DH arm_dh_[6];       //486 = 24
   TransMatrix base_;   //9+3 = 12
   Posture post;        //4/2 = 2
}FORCE_PARA_ROBOTIC_t;//动力学参数

typedef struct PARA_READ_INFO
{
    FORCE_PARA_ROBOTIC_t robotic_para;     //dh base 38
    FORCE_PARA_IMPEDANCE_t impendance_para;//M D K   18
    FORCE_PARA_PID_t pid_para_imped[6];          //pid     18
    FORCE_PARA_PID_t pid_para_adnit[6];       //adnittance pid
    double para_rsv[35];
}PARA_READ_INFO_t;

/********************Command**********************/
typedef struct
{
    int32_t cmd;
    int32_t param1;
    int32_t param2;
    int32_t param3;
    int32_t param4;
    int32_t param5;
    int32_t param6;
    int32_t param7;
}ServoCoreProcessCall_t;
typedef struct
{
    int32_t param1;
    int32_t param2;
    int32_t param3;
    int32_t param4;
    int32_t param5;
    int32_t param6;
    int32_t param7;
    int32_t param8;
}ForceCoreProcessBack_t;

typedef enum FORCE_WORK_MODE
{
    NO_MODE = 0,
    IMPEDANCE_MODE = 1,
    ADMITTANCE_MODE = 2,
}FORCE_WORK_MODE_e;

typedef enum FORCE_WORK_SUB_MODE
{
    NO_SUB_MODE = 0,
    TEACH = 1,
    LEARN = 2,
}FORCE_WORK_SUB_MODE_e;

typedef enum{
    /*state machine control*/
    CMD_ENABLE = 0x01,
    CMD_WORKING = 0x02,
    CMD_IDLE = 0x03,
    CMD_ABORT = 0x04,   //discard instruction
    CMD_CONTINUE = 0x05,
    CMD_DISABLE = 0x06,
    CMD_RESET_FAULT = 0x07,

    /*change mode*/
    CMD_SWITCH_MODE = 0x08,

    /*dignose config*/
    CMD_DIGNOSE_CONFIG = 0x09,

    CMD_SET_PARA = 0x0a,
    CMD_SET_INS = 0x0b,
    CMD_GET_RES = 0x0c,

    CMD_SHAKE_REQ = 0x0d,
    CMD_SHAKE_BIND = 0x0e,
    CMD_HEART_INC = 0x0f,
    CMD_SERVO_MAX = CMD_DIGNOSE_CONFIG + 1,

}SERVO_CMD_e;

/******************Instruction*****************/
typedef struct{
    Vector6 in_pos_given;
    Vector6 in_vel_given;
    Vector6 in_acc_given;
    Vector6 in_force_given;

    Vector6 in_pos_back;
    Vector6 in_vel_back;
    Vector6 in_acc_back;
    Vector6 in_force_back;
    double rsv_data[16];
}FORCE_INSTRUCTION_INFO_t; // 512 Byte

/**************************feedback*************************/
typedef struct{
    int err_code;
    int err_history[4];//
    int state;
    int mode;
    int sub_mode;
    Vector6 theta;
    Vector6 force;
    double dignose_var[8][6];
}FORCE_BACK_INFO_t;//512Bytes

/*************************************************************/
typedef buffer_info_t SERVO_COMM_RINGS_BUFF_STRUCT;

extern void Force_Servo_Comm_Init(void);
extern int Force_Servo_Comm_Get_Para_UpdateFlag(void);
extern void Force_Servo_Comm_Set_Para_UpdateFlag(int val);

extern void Force_Servo_Comm_ShakeHand_Request(void);
extern void Force_Servo_Comm_ShakeHand_Bind(void);
extern void Force_Servo_Comm_ServoHeart_Inc(void);
extern int Force_Servo_Comm_Get_ForceHeart(void);

extern void Force_Servo_Comm_Cmd_Enable(void);//change state from init to idle
extern void Force_Servo_Comm_Cmd_Working(void);//change state from idle to working
extern void Force_Servo_Comm_Cmd_Idle(void);//change state from working to idle
extern void Force_Servo_Comm_Cmd_Abort(void);//change state from working to abort
extern void Force_Servo_Comm_Cmd_Continue(void);
extern void Force_Servo_Comm_Cmd_Disable(void);//change state from init to idle

extern void Force_Servo_Comm_Cmd_ResetFault(void);
extern void Force_Servo_Comm_Cmd_Mode(int mode, int submode);
extern void Force_Servo_Comm_Cmd_DignoseCfg(FORCE_DIGNOSE_VAR_e *ptr);

extern void Force_Servo_Comm_Set_Para(PARA_READ_INFO_t *para_ptr);
extern int Force_Servo_Comm_Set_Instruct(FORCE_INSTRUCTION_INFO_t *push_ptr,int32_t length);
extern int Force_Servo_Comm_Get_Result(FORCE_BACK_INFO_t *pull_ptr);

#endif


