/**
 * @file pid.c
 * @author ZheWana
 * @brief PID控制器源文件
 * @version 0.1
 * @date 2022-03-12
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "./pid.h"
#include "main.h"
#include "math.h"

/**
 * @brief PID实现函数
 * @param pid PID句柄
 * @param input PID输入量
 * @return float PID控制器输出
 */
float PID_Realize(Pid_t *pid, float input) {
    pid->ctr.cur = input;

    pid->error.cur = pid->ctr.aim - pid->ctr.cur;
    pid->error.sum += pid->error.cur;
    pid->error.bia = pid->error.cur - pid->error.pre;
    pid->error.pre = pid->error.cur;

    pid->ctr.pre = pid->ctr.cur;
    return pid->kp * pid->error.cur + pid->ki * pid->error.sum + pid->kd * pid->error.bia;
}

/**
 * @brief PID角度闭环的特殊实现
 * @param pid PID句柄
 * @param input PID输入量
 * @return float PID控制器输出
 */
float PID_RealizeForAngle(Pid_t *pid, float input) {
    pid->ctr.cur = input;

    pid->error.cur = pid->ctr.aim - pid->ctr.cur;
    if (fabsf(pid->error.cur) > M_PI && fabsf(pid->error.cur) < 2 * M_PI) { // 角度溢出处理
        if (pid->error.cur > 0) {
            pid->error.cur -= 2 * M_PI;
        } else if (pid->error.cur < 0) {
            pid->error.cur += 2 * M_PI;
        }
    }
    pid->error.sum += pid->error.cur;
    pid->error.bia = pid->error.cur - pid->error.pre;
    pid->error.pre = pid->error.cur;

    pid->ctr.pre = pid->ctr.cur;
    return pid->kp * pid->error.cur + pid->ki * pid->error.sum + pid->kd * pid->error.bia;
}

/**
 * @brief PID初始化
 * @param pid PID句柄
 * @param kp 比例参数
 * @param ki 积分参数
 * @param kd 微分参数
 */
void PID_Init(Pid_t *pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

/**
 * @brief PID内部运算量重置
 * @param pid PID句柄
 */
void PID_Reset(Pid_t *pid) {
    pid->ctr.cur = 0;
    pid->ctr.pre = 0;
    pid->error.cur = 0;
    pid->error.pre = 0;
    pid->error.bia = 0;
    pid->error.sum = 0;
}

/**
 * @brief 设定PID目标值
 * @param pid PID句柄
 * @param aim PID目标值
 */
void PID_SetAim(Pid_t *pid, float aim) {
    pid->ctr.aim = aim;
}
