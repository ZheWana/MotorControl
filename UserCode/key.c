/**
 * @file key.c
 * @author ZheWana(zhewana.cn)
 * @brief 
 * @date 2023/4/10
  */
#include "CommonKey/comKey.h"
#include "utils.h"

/**
 * @brief 渲染器Menu面板参数定义
 */
float MenuItem_y_w[2][2] = {
        0, 5 * 15,
        23, 8 * 15,
};

/**
 * @brief 渲染器Speed面板参数定义
 */
float SpeedItem_y_w[2][2] = {
        0, 3 * 10,
        13, 4 * 10,
};

/**
 * @brief 渲染器Position面板参数定义
 */
float PositionItem_y_w[2][2] = {
        0, 3 * 10,
        13, 4 * 10,
};

void ComKey_HoldTriggerCallback(comkey_t *key) {
    if (mInstance.menuSelect == Speed) {
        if (key == &key1) {
            mInstance.uiAim = mInstance.uiAim + 10 > 70 ? 70 : mInstance.uiAim + 10;
        } else if (key == &key2) {
            mInstance.uiAim = mInstance.uiAim - 10 < -70 ? -70 : mInstance.uiAim - 10;
        }
    } else if (mInstance.menuSelect == Position) {
        if (key == &key1) {
            mInstance.uiAim = mInstance.uiAim + (float) 10;
        } else if (key == &key2) {
            mInstance.uiAim = mInstance.uiAim - (float) 10;
        }
    }
}

void ComKey_MultipleClickCallback(comkey_t *key) {
    if (mInstance.menuSelect == Speed) {
        if (key == &key1) {
            mInstance.uiAim =
                    mInstance.uiAim + (float) key->clickCnt > 70 ? 70 : mInstance.uiAim + (float) key->clickCnt;
        } else if (key == &key2) {
            mInstance.uiAim =
                    mInstance.uiAim - (float) key->clickCnt < -70 ? -70 : mInstance.uiAim - (float) key->clickCnt;
        }
    } else if (mInstance.menuSelect == Position) {
        if (key == &key1) {
            mInstance.uiAim = mInstance.uiAim + (float) key->clickCnt;
        } else if (key == &key2) {
            mInstance.uiAim = mInstance.uiAim - (float) key->clickCnt;
        }
    }
}

void ComKey_KeyPressCallback(comkey_t *key) {

    switch (mInstance.menuSelect) {
        static int selected = 0;

        case Menu:
            if (key == &key1) {
                selected = selected + 1 >= sizeof(MenuItem_y_w) / sizeof(MenuItem_y_w[0]) ? 0 : selected + 1;

                PID_SetAim(&mInstance.fyPID, MenuItem_y_w[selected][0]);
                PID_SetAim(&mInstance.fwPID, MenuItem_y_w[selected][1]);
            } else if (key == &key2) {
                selected = selected - 1 < 0 ?
                           (int) (sizeof(MenuItem_y_w) / sizeof(MenuItem_y_w[0]) - 1) : selected - 1;

                PID_SetAim(&mInstance.fyPID, MenuItem_y_w[selected][0]);
                PID_SetAim(&mInstance.fwPID, MenuItem_y_w[selected][1]);
            } else if (key == &key3) {// Confirm
                float **itemPtr = NULL;
                if (selected == 0) {
                    mInstance.menuSelect = Speed;

                    PID_SetAim(&mInstance.fyPID, SpeedItem_y_w[selected][0]);
                    PID_SetAim(&mInstance.fwPID, SpeedItem_y_w[selected][1]);

                    PID_Reset(&mInstance.fyPID);
                    PID_Reset(&mInstance.fwPID);
                    PID_Reset(&mInstance.mPID);
                } else if (selected == 1) {
                    selected = 0;
                    mInstance.menuSelect = Position;

                    PID_SetAim(&mInstance.fyPID, PositionItem_y_w[selected][0]);
                    PID_SetAim(&mInstance.fwPID, PositionItem_y_w[selected][1]);

                    PID_Reset(&mInstance.fyPID);
                    PID_Reset(&mInstance.fwPID);
                    PID_Reset(&mInstance.mPID);
                    PID_Reset(&mInstance.pPID);

                    mInstance.realPosition = 0;
                }

            }
            break;

        case Speed:
            if (key == &key3) {// Confirm
                PID_SetAim(&mInstance.mPID, (float) mInstance.uiAim);
            } else if (key == &key4) {// Cancel
                mInstance.menuSelect = Menu;
                mInstance.uiAim = 0;

                PID_SetAim(&mInstance.mPID, 0);
                PID_Reset(&mInstance.mPID);

                selected = 0;
                PID_SetAim(&mInstance.fyPID, MenuItem_y_w[selected][0]);
                PID_SetAim(&mInstance.fwPID, MenuItem_y_w[selected][1]);

                PID_Reset(&mInstance.fyPID);
                PID_Reset(&mInstance.fwPID);
            }
            break;

        case Position:
            if (key == &key3) {// Confirm
                PID_SetAim(&mInstance.pPID, (float) mInstance.uiAim);
            } else if (key == &key4) {// Cancel
                mInstance.menuSelect = Menu;
                mInstance.uiAim = 0;

                PID_SetAim(&mInstance.mPID, 0);
                PID_Reset(&mInstance.mPID);
                PID_SetAim(&mInstance.pPID, 0);
                PID_Reset(&mInstance.pPID);

                selected = 0;
                PID_SetAim(&mInstance.fyPID, MenuItem_y_w[selected][0]);
                PID_SetAim(&mInstance.fwPID, MenuItem_y_w[selected][1]);

                PID_Reset(&mInstance.fyPID);
                PID_Reset(&mInstance.fwPID);
            }
            break;
    }


}