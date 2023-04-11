/**
 * @file pid.h
 * @author ZheWana
 * @brief PID控制器头文件
 * @version 0.1
 * @date 2022-03-12
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef PID_H
#define PID_H

#ifndef M_PI
#define M_PI 3.1415926535
#endif

typedef struct {
    float kp;
    float ki;
    float kd;

    struct ctr {
        float cur;
        float pre;
        float aim;
    } ctr;

    struct error {
        float cur;
        float pre;
        float sum;
        float bia;
    } error;

} Pid_t;

/**
 * @brief PID初始化函数
 * @param pid pid句柄
 * @param kp 比例参数
 * @param ki 积分参数
 * @param kd 微分参数
 */
void PID_Init(Pid_t *pid, float kp, float ki, float kd);

/**
 * @brief PID设定目标值
 * @param pid pid句柄
 * @param aim 目标值
 */
void PID_SetAim(Pid_t *pid, float aim) ;

/**
 * @brief PID重置函数
 * @param pid pid句柄
 * @note 重置函数不对PID参数和目标值进行重置，仅对运算中间量进行重置
 */
void PID_Reset(Pid_t *pid);

/**
 * @brief 一般PID实现函数
 * @param pid pid句柄
 * @param input 当前输入量
 * @return PID输出量
 */
float PID_Realize(Pid_t* pid, float input);

/**
 * @brief 带有角度溢出的PID实现
 * @param pid pid句柄
 * @param input 当前输入量
 * @return PID输出量
 * @note 角度使用弧度制
 */
float PID_RealizeForAngle(Pid_t *pid, float input);

#endif // PID_H