#include "device.hpp"
#include "can.h"
#include "dji.hpp"
#include "vesc.hpp"
#include "cmsis_os2.h"

namespace Device
{

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

constexpr motors::DJIMotor::Config motor_wheel_speed_config[4] = {
        {
                .hcan           = &hcan1,
                .type           = motors::DJIMotor::Type::M2006_C610,
                .id1            = 1,
                .reverse        = false,
                .reduction_rate = 2.0f, // 舵轮底盘2006电机外接减速比为2:1
        },
        {
                .hcan           = &hcan1,
                .type           = motors::DJIMotor::Type::M2006_C610,
                .id1            = 2,
                .reverse        = true,
                .reduction_rate = 2.0f, // 舵轮底盘2006电机外接减速比为2:1
        },
        {
                .hcan           = &hcan1,
                .type           = motors::DJIMotor::Type::M2006_C610,
                .id1            = 3,
                .reverse        = true,
                .reduction_rate = 2.0f, // 舵轮底盘2006电机外接减速比为2:1
        },
        {
                .hcan           = &hcan1,
                .type           = motors::DJIMotor::Type::M2006_C610,
                .id1            = 4,
                .reverse        = false,
                .reduction_rate = 2.0f, // 舵轮底盘2006电机外接减速比为2:1
        },
};

constexpr motors::VESCMotor::Config motor_wheel_dir_config[4] = {
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
        motor::motor_wheel_speed[i] = new motors::DJIMotor(motor_wheel_speed_config[i]);
        motor::motor_wheel_dir[i]   = new motors::VESCMotor(motor_wheel_dir_config[i]);
    }
}

void APP_DEVICE_Init()
{
    can_init();
    motors_init();
}

void update_1kHz()
{
    motors::DJIMotor::SendIqCommand(&hcan1, motors::DJIMotor::IqSetCMDGroup::IqCMDGroup_1_4);
    motors::DJIMotor::SendIqCommand(&hcan1, motors::DJIMotor::IqSetCMDGroup::IqCMDGroup_5_8);
    motors::DJIMotor::SendIqCommand(&hcan2, motors::DJIMotor::IqSetCMDGroup::IqCMDGroup_1_4);
    motors::DJIMotor::SendIqCommand(&hcan2, motors::DJIMotor::IqSetCMDGroup::IqCMDGroup_5_8);
}

bool isAllConnected()
{
    constexpr auto def_and_connected = [](auto a) { return a && a->isConnected(); };

    for (auto& m : motor::motor_wheel_speed)
        if (!def_and_connected(m))
            return false;
    for (auto& m : motor::motor_wheel_dir)
        if (!def_and_connected(m))
            return false;
    return true;
}

void waitAllConnected()
{
    while (!isAllConnected())
        osDelay(1);
}

} // namespace Device