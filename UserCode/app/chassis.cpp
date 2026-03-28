#include "chassis.hpp"
#include "Special_Steering4.hpp"
#include "device.hpp"
#include "motor_pos_controller.hpp"
#include "pid_motor.hpp"
#include "motor_if.hpp"

using controllers::ControlMode;

static PIDMotor::Config motor_wheeldir_velpid = {.Kp             = 25.0f,
                                                 .Ki             = 0.15f,
                                                 .Kd             = 20.0f,
                                                 .abs_output_max = 8000};

static PIDMotor::Config motor_wheeldir_pospid = {.Kp             = 25.0f,
                                                 .Ki             = 0.15f,
                                                 .Kd             = 20.0f,
                                                 .abs_output_max = 30};

MotorVelController* motor_wheelspeed_velctrl[4];
MotorPosController* motor_wheeldir_posctrl[4];
MotorVelController* motor_wheeldir_velctrl[4];
Special_Steering4*  chassis_;

void Chassis_Init()
{
    for (size_t i = 0; i < 4; i++)
    {
        motor_wheelspeed_velctrl[i] =
                new MotorVelController(motor_wheel_speed[i],
                                       {.ctrl_mode = ControlMode::InternalVel});
        motor_wheeldir_posctrl[i] = new MotorPosController(motor_wheel_dir[i],
                                                           {.position_pid = motor_wheeldir_pospid});
        motor_wheeldir_velctrl[i] = new MotorVelController(motor_wheel_dir[i],
                                                           {.pid = motor_wheeldir_velpid});
    }
#warning ("现在底盘的光电门还没装上去，自动校准暂不开启")
    chassis_ = new Special_Steering4(Special_Steering4::Config{
            .radius     = 100.0f,
            .distance_x = 63.0f,
            .distance_y = 200.0f,

    });
}