#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "vesc.hpp"
#include "dji.hpp"

extern motors::DJIMotor* motor_wheel_speed[4]; // 底盘轮子电机
extern motors::VESCMotor* motor_wheel_dir[4];   // 底盘舵向电机

void APP_DEVICE_Init();

#endif // _DEVICE_H_