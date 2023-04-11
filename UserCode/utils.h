/**
 * @file utils.h
 * @author ZheWana(zhewana.cn)
 * @brief 
 * @date 2023/4/10
  */
#ifndef MOTORCONTROL_UTILS_H
#define MOTORCONTROL_UTILS_H

#include "tim.h"
#include "usart.h"
#include "PID/pid.h"
#include "CommonKey/comKey.h"

typedef struct motor {
    float realSpeed, realPosition;
    float uiAim;
    int32_t preCNT, curCNT;

    // 速度环电机控制
    Pid_t mPID;
    // 位置环电机控制
    Pid_t pPID;

    // 菜单状态机
    enum menuSelect {
        Menu,
        Speed,
        Position
    } menuSelect;

    // 选择框位置效果
    Pid_t fyPID;
    // 选择框宽度效果
    Pid_t fwPID;
} motor_t;

extern motor_t mInstance;
extern float MenuItem_y_w[2][2];
extern comkey_t key1, key2, key3, key4;

#endif //MOTORCOMTROL_UTILS_H
