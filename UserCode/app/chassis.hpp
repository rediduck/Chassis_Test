#ifndef _CHASSIS_H_
#define _CHASSIS_H_

#include "Special_Steering4.hpp"
#include "motor_pos_controller.hpp"
#include "motor_vel_controller.hpp"
#include "gpio_driver.h"
#include "JustEncoder.hpp"
#include "Master.hpp"

#pragma once

namespace Chassis
{

using chassis::controller::Master;
using chassis::loc::JustEncoder;
using chassis::motion::Special_Steering4;
using controllers::MotorPosController;
using controllers::MotorVelController;

#define GPIO_FRONT_RIGHT (GPIO_t{GPIOB, GPIO_PIN_0})
#define GPIO_FRONT_LEFT  (GPIO_t{GPIOB, GPIO_PIN_1})
#define GPIO_REAR_LEFT   (GPIO_t{GPIOB, GPIO_PIN_2})
#define GPIO_REAR_RIGHT  (GPIO_t{GPIOB, GPIO_PIN_3})

void APP_CHASSIS_Init();
} // namespace Chassis

#endif // _CHASSIS_H_