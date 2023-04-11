/**
 * @file comKey.c
 * @author ZheWana
 * @date 2022/3/6 006
 */

#include "comKey.h"
#include "main.h"

comkey_t key1, key2, key3, key4;

__attribute__((weak)) void ComKey_SyncValue(comkey_t *key) {
    // if your key was pressed,"key->val" should be 1.
    if (key == &key1) {
        key->val = !HAL_GPIO_ReadPin(KEY_1_GPIO_Port, KEY_1_Pin);
    } else if (key == &key2) {
        key->val = !HAL_GPIO_ReadPin(KEY_2_GPIO_Port, KEY_2_Pin);
    } else if (key == &key3) {
        key->val = !HAL_GPIO_ReadPin(KEY_3_GPIO_Port, KEY_3_Pin);
    } else if (key == &key4) {
        key->val = !HAL_GPIO_ReadPin(KEY_4_GPIO_Port, KEY_4_Pin);
    }

    // IMPORTANT！！！DO NOT MODIFIED!!!
    key->preVal = key->val;
}

__attribute__((weak)) void ComKey_FirstLongTriggerCallback(comkey_t *key) {
}

__attribute__((weak)) void ComKey_LongHoldCallback(comkey_t *key) {
}

__attribute__((weak)) void ComKey_HoldTriggerCallback(comkey_t *key) {
}

__attribute__((weak)) void ComKey_MultipleClickCallback(comkey_t *key) {
}

__attribute__((weak)) void ComKey_KeyReleaseCallback(comkey_t *key) {
}

__attribute__((weak)) void ComKey_KeyPressCallback(comkey_t *key) {
}

/*************** DO NOT MODIFY THE FOLLOWING CODE *******************/

static pcomkey_t head = NULL, tail = NULL;
static uint8_t ITPeriod = 1;

void ComKey_Init(comkey_t *key, int pollingPeriod) {
    ITPeriod = pollingPeriod;
    key->state = Release;
    key->next = NULL;

    if (head == NULL) {
        head = key;
        tail = key;
    } else {
        tail->next = key;
        tail = tail->next;
    }
}

void ComKey_Handler() {

    for (pcomkey_t key = head; key != NULL; key = key->next) {
        //键值同步
        ComKey_SyncValue(key);

        //按下计时
        if (!key->val) {
            if (key->state == LongHold) {
                key->holdTime = key->preTimer;
            }
            key->preTimer = 0;
        }
        if (key->preVal & key->val) {
            key->preTimer += ITPeriod;
        }
        //间隔计时
        if (key->state == MultiClick) {
            key->intervalTimer += ITPeriod;
        } else {
            key->intervalTimer = 0;
        }

        //事件生成
        switch (key->state) {
            case Release:
                key->clickCnt = 0;

                if (key->val) {
                    key->state = PrePress;
                }
                break;
            case PrePress:

                if (!key->val) {
                    key->state = Release;
                } else if (key->preTimer > COMKEY_ClickThreshold) {
                    key->state = Prelong;
                    ComKey_KeyPressCallback(key);
                }
                break;
            case Prelong:

                if (!key->val) {
                    key->state = MultiClick;
                    key->clickCnt++;
                } else if (key->preTimer > COMKEY_HoldThreshold) {
                    key->state = LongHold;
                    key->triggerTimer = COMKEY_HoldTriggerThreshold;
                    ComKey_FirstLongTriggerCallback(key);
                }
                break;
            case LongHold: {
                if (key->triggerTimer > 0)
                    key->triggerTimer -= ITPeriod;
                else {
                    key->triggerTimer = COMKEY_HoldTriggerThreshold;
                    ComKey_HoldTriggerCallback(key);
                }

                if (!key->val) {
                    ComKey_LongHoldCallback(key);
                    ComKey_KeyReleaseCallback(key);
                    key->state = Release;
                }
            }
                break;
            case MultiClick:

                if (key->intervalTimer > COMKEY_IntervalVal) {
                    ComKey_MultipleClickCallback(key);
                    ComKey_KeyReleaseCallback(key);
                    key->state = Release;
                } else if (key->preTimer > COMKEY_ClickThreshold) {
                    key->state = Prelong;
                }
                break;

            default:
#ifdef USE_HAL_DRIVER
                Error_Handler();
#endif // USE_HAL_DRIVER
                break;
        }
    }
}