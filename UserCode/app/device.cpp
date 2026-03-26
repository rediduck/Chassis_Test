#include "device.hpp"
#include "can.h"
#include "dji.hpp"
#include "vesc.hpp"

motors::DJIMotor*  motor_speedwheel[4]; // 底盘轮子电机
motors::VESCMotor* motor_dirwheel[4];   // 底盘舵向电机

static void can_init()
{
    // 1. 配置 CAN 滤波器
    motors::DJIMotor::CAN_FilterInit(&hcan1, 0);
    CAN_RegisterCallback(&hcan1, motors::DJIMotor::CANBaseReceiveCallback);
    motors::DJIMotor::CAN_FilterInit(&hcan2, 14);
    CAN_RegisterCallback(&hcan2, motors::DJIMotor::CANBaseReceiveCallback);

    HAL_CAN_RegisterCallback(&hcan1, HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID, CAN_Fifo0ReceiveCallback);
    CAN_Start(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_RegisterCallback(&hcan2, HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID, CAN_Fifo0ReceiveCallback);
    CAN_Start(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}

constexpr motors::DJIMotor::Config motor_speedwheel_config[4] = {
    {
            .hcan    = &hcan1,
            .type    = motors::DJIMotor::Type::M2006_C610,
            .id1     = 1,
            .reverse = false,
    },
    {
            .hcan    = &hcan1,
            .type    = motors::DJIMotor::Type::M2006_C610,
            .id1     = 2,
            .reverse = true,
    },
    {
            .hcan    = &hcan1,
            .type    = motors::DJIMotor::Type::M2006_C610,
            .id1     = 3,
            .reverse = true,
    },
    {
            .hcan    = &hcan1,
            .type    = motors::DJIMotor::Type::M2006_C610,
            .id1     = 4,
            .reverse = false,
    },
};

constexpr motors::VESCMotor::Config motor_dirwheel_config[4] = {
    {
            .hcan       = &hcan2,
            .id         = 0x01,
            .electrodes = 14,
    },
    {
            .hcan       = &hcan2,
            .id         = 0x02,
            .electrodes = 14,
    },
    {
            .hcan       = &hcan2,
            .id         = 0x03,
            .electrodes = 14,
    },
    {
            .hcan       = &hcan2,
            .id         = 0x04,
            .electrodes = 14,
    },
};

static void motors_init()
{
    for (size_t i = 0; i < 4; ++i)
    {
        motor_speedwheel[i] = new motors::DJIMotor(motor_speedwheel_config[i]);
        motor_dirwheel[i]   = new motors::VESCMotor(motor_dirwheel_config[i]);
    }
}

void APP_DEVICE_Init()
{
    can_init();
    motors_init();
}
