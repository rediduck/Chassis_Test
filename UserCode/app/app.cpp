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

float vx = 1.0f, vy = 0.0f, wz = 90.0f;

osThreadId_t         myHandle;
const osThreadAttr_t my_attributes = {
        .name       = "my",
        .stack_size = 512 * 4,
        .priority   = (osPriority_t)osPriorityRealtime7,
};

extern "C" void TIM_Callback_1kHz(TIM_HandleTypeDef* htim)
{
    service::Watchdog::EatAll();
    Chassis::update_1kHz();
    Device::update_1kHz();
}

extern "C" void TIM_Callback_100Hz(TIM_HandleTypeDef* htim)
{
    Chassis::update_100Hz();
}

extern "C" void my(void* argument)
{
    float _vx = 0, _vy = 0, _wz = 0;
    while (1)
    {
        if (_vx != vx || _vy != vy || _wz != wz)
        {
            _vx = vx;
            _vy = vy;
            _wz = wz;
            Chassis::chassis_ctrl_->setVelocityInBody({.vx = vx, .vy = vy, .wz = wz}, true);
        }
        osDelay(100);
    }
}

extern "C" void Init(void* argument)
{
    /* 初始化代码 */

    Device::APP_DEVICE_Init();
    Chassis::APP_CHASSIS_Init();
    // 启动定时器
    HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID, TIM_Callback_1kHz);
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_TIM_RegisterCallback(&htim13, HAL_TIM_PERIOD_ELAPSED_CB_ID, TIM_Callback_100Hz);
    HAL_TIM_Base_Start_IT(&htim13);

    // Device::waitAllConnected();
    osDelay(5000);

    Chassis::chassis_ctrl_->enable();
    osThreadNew(my, NULL, &my_attributes);
    /* 初始化完成后退出线程 */
    osThreadExit();
}