/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "PID/pid.h"
#include "utils.h"
#include "printf.h"
#include "u8g2/u8g2.h"
#include "u8g2/u8x8.h"
#include "CommonKey/comKey.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
u8g2_t u8g2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

uint8_t u8x8_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg,
                            U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
    HAL_TIM_Base_Start_IT(&htim11);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_2);

    PID_Init(&mInstance.mPID, 0, 0.003f, 0);
    PID_Init(&mInstance.pPID, 5.0f, 0.001f, 0);

    PID_Init(&mInstance.fyPID, 0.07f, 0.002f, 0);
    PID_Init(&mInstance.fwPID, 0.07f, 0.002f, 0);

    ComKey_Init(&key1, 1);
    ComKey_Init(&key2, 1);
    ComKey_Init(&key3, 1);
    ComKey_Init(&key4, 1);

    u8g2_Setup_ssd1306_128x64_noname_1(&u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi,
                                       u8x8_gpio_and_delay);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    PID_SetAim(&mInstance.fyPID, 0);
    PID_SetAim(&mInstance.fwPID, 5 * 15);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1) {
        char buff[256] = {0};

        printf("data:%f,%f,%f,%f\n", mInstance.mPID.ctr.aim, mInstance.mPID.ctr.cur,
               mInstance.pPID.ctr.aim, mInstance.pPID.ctr.cur);

        switch (mInstance.menuSelect) {
            case Menu:
                u8g2_FirstPage(&u8g2);
                do {
                    u8g2_SetFont(&u8g2, u8g2_font_courR18_tf);
                    u8g2_DrawStr(&u8g2, 2, 18, "Speed");
                    u8g2_DrawStr(&u8g2, 2, 36 + 5, "Position");
                    u8g2_SetFont(&u8g2, u8g2_font_courR10_tf);
                    u8g2_DrawStr(&u8g2, 2, 36 + 20, "B20040609");

                    // Frame renderer
                    u8g2_DrawFrame(&u8g2, 2, (uint16_t) mInstance.fyPID.ctr.cur,
                                   (uint16_t) mInstance.fwPID.ctr.cur, 20);
                } while (u8g2_NextPage(&u8g2));
                break;

            case Speed:
                u8g2_FirstPage(&u8g2);
                do {
                    u8g2_SetFont(&u8g2, u8g2_font_courR10_tf);
                    sprintf(buff, "Aim:%.2f", mInstance.uiAim);
                    u8g2_DrawStr(&u8g2, 2, 12, buff);
                    sprintf(buff, "Real:%.2f", mInstance.mPID.ctr.cur);
                    u8g2_DrawStr(&u8g2, 2, 24, buff);

                    // Frame renderer
                    u8g2_DrawFrame(&u8g2, 0, (uint16_t) mInstance.fyPID.ctr.cur,
                                   (uint16_t) mInstance.fwPID.ctr.cur, 14);

                    // Aim progress bar renderer
                    u8g2_DrawFrame(&u8g2, 0, 50, 128, 6);
                    if (mInstance.uiAim >= 0)
                        u8g2_DrawBox(&u8g2, 0, 50, (uint16_t) ((float) mInstance.mPID.ctr.aim / 70.0f * 128), 6);
                    else
                        u8g2_DrawBox(&u8g2, 128 - (uint16_t) (fabsf(mInstance.mPID.ctr.aim) / 70.0f * 128), 50,
                                     (uint16_t) (fabsf(mInstance.mPID.ctr.aim) / 70.0f * 128), 6);

                    // Real progress bar renderer
                    u8g2_DrawFrame(&u8g2, 0, 58, 128, 6);
                    if (mInstance.uiAim >= 0)
                        u8g2_DrawBox(&u8g2, 0, 58, (uint16_t) ((float) mInstance.mPID.ctr.cur / 70.0f * 128), 6);
                    else
                        u8g2_DrawBox(&u8g2, 128 - (uint16_t) (fabsf(mInstance.mPID.ctr.cur) / 70.0f * 128), 58,
                                     (uint16_t) (fabsf(mInstance.mPID.ctr.cur) / 70.0f * 128), 6);
                } while (u8g2_NextPage(&u8g2));
                break;

            case Position:
                u8g2_FirstPage(&u8g2);
                do {
                    u8g2_SetFont(&u8g2, u8g2_font_courR10_tf);
                    sprintf(buff, "Aim:%.2f", mInstance.uiAim);
                    u8g2_DrawStr(&u8g2, 2, 12, buff);
                    sprintf(buff, "Real:%.2f", mInstance.pPID.ctr.cur);
                    u8g2_DrawStr(&u8g2, 2, 24, buff);

                    // Frame renderer
                    u8g2_DrawFrame(&u8g2, 0, (uint16_t) mInstance.fyPID.ctr.cur,
                                   (uint16_t) mInstance.fwPID.ctr.cur, 14);

                    // Aim progress bar renderer
                    u8g2_DrawFrame(&u8g2, 0, 50, 128, 6);
                    if (mInstance.pPID.ctr.aim == 0)
                        u8g2_DrawBox(&u8g2, 0, 50, 0, 6);
                    else
                        u8g2_DrawBox(&u8g2, 0, 50, 64, 6);
                    // Real progress bar renderer
                    u8g2_DrawFrame(&u8g2, 0, 58, 128, 6);
                    u8g2_DrawBox(&u8g2, 0, 58,
                                 (uint16_t) (mInstance.pPID.ctr.cur / mInstance.pPID.ctr.aim * 64), 6);
                } while (u8g2_NextPage(&u8g2));
                break;
        }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
