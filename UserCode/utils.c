/**
 * @file utils.c
 * @author ZheWana(zhewana.cn)
 * @brief 
 * @date 2023/4/10
  */
#include "utils.h"
#include "math.h"

motor_t mInstance;

void _putchar(char character) {
    HAL_UART_Transmit(&huart1, (uint8_t * ) & character, 1, HAL_MAX_DELAY);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim11) {
        // Speed Calculate
        mInstance.preCNT = (int32_t) mInstance.curCNT;
        mInstance.curCNT = (int32_t)TIM2->CNT;
        mInstance.realSpeed = (float) (mInstance.curCNT - mInstance.preCNT);
        mInstance.realPosition += mInstance.realSpeed * 0.01f;

        // Optional position PID calculate
        if (mInstance.menuSelect == Position) {
            float pidOut = PID_Realize(&mInstance.pPID, mInstance.realPosition);
            PID_SetAim(&mInstance.mPID, pidOut > 70 ? 70 : pidOut < -70 ? -70 : pidOut);
        }

        // Speed PID calculate
        float pidOut = PID_Realize(&mInstance.mPID, mInstance.realSpeed);

        // Driver layer
        if (pidOut < 0) {
            TIM1->CCR3 = 0;
            TIM1->CCR2 = (uint32_t)(fabsf(pidOut) * (TIM1->ARR - 1.0f)) < TIM1->ARR - 1 ?
                         (uint32_t)(fabsf(pidOut) * (TIM1->ARR - 1.0f)) : TIM1->ARR - 1;
        } else {
            TIM1->CCR2 = 0;
            TIM1->CCR3 = (uint32_t)(fabsf(pidOut) * (TIM1->ARR - 1.0f)) < TIM1->ARR - 1 ?
                         (uint32_t)(fabsf(pidOut) * (TIM1->ARR - 1.0f)) : TIM1->ARR - 1;
        }

        Pid_t *tempPID = mInstance.menuSelect == Position ? &mInstance.pPID : &mInstance.mPID;
        if (fabsf(tempPID->ctr.aim - tempPID->ctr.cur) / fabsf(tempPID->ctr.aim) < 0.05f) {
            HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, 1);
            HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, 0);
        } else {
            HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, 0);
            HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, 1);
        }

        ////////////////////////////  Renderer /////////////////////////////////
        mInstance.fyPID.ctr.cur += PID_Realize(&mInstance.fyPID, mInstance.fyPID.ctr.cur);
        mInstance.fwPID.ctr.cur += PID_Realize(&mInstance.fwPID, mInstance.fwPID.ctr.cur);
    }
}