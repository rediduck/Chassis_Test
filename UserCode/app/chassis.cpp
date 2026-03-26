#include "chassis.hpp"
#include "Special_Steering4.hpp"
#include "device.hpp"
#include "utils.h"
#include "pid_motor.hpp"

static PIDMotor::Config motor_speedwheel_pid = {
    .Kp = 25.0f, .Ki = 0.15f, .Kd = 20.0f, .abs_output_max = 8000
};

controllers::MotorVelController* motor_vel_ctrl[4];

void APP_Chassis_Init()
{
    using chassis::motion::Special_Steering4;
    using controllers::MotorPosController;
    using controllers::MotorVelController;

    for (size_t i = 0; i < 4; i++)
    {
    }
}