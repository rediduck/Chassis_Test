/**
 * @file    app.h
 * @author  rediduck
 * @date    2026-03-25
 */
#include "app.hpp"
#include "chassis.hpp"
#include "cmsis_os2.h"
#include "device.hpp"
#include "tim.h"
#include "main.h"

extern "C" void TIM_Callback_1kHz(TIM_HandleTypeDef* htim) {}

extern "C" void TIM_Callback_100Hz(TIM_HandleTypeDef* htim) {}

extern "C" void Init(void* argument)
{
    /* 初始化代码 */

    APP_DEVICE_Init();

    // 启动定时器
    HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID, TIM_Callback_1kHz);
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_TIM_RegisterCallback(&htim13, HAL_TIM_PERIOD_ELAPSED_CB_ID, TIM_Callback_100Hz);
    HAL_TIM_Base_Start_IT(&htim13);
    /* 初始化完成后退出线程 */
    osThreadExit();
}