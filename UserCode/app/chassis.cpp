#include "chassis.hpp"
#include "Special_Steering4.hpp"
#include "device.hpp"
#include "motor_pos_controller.hpp"
#include "pid_motor.hpp"
#include "motor_if.hpp"

namespace Chassis
{
using controllers::ControlMode;

static PIDMotor::Config motor_wheeldir_velpid = {.Kp             = 500.0f,
                                                 .Ki             = 0.1f,
                                                 .Kd             = 0.0f,
                                                 .abs_output_max = 8000};

static PIDMotor::Config motor_wheeldir_pospid = {.Kp             = 2.0f,
                                                 .Ki             = 0.0f,
                                                 .Kd             = 0.2f,
                                                 .abs_output_max = 400};

MotorVelController* motor_wheelspeed_velctrl[4] = {nullptr};
MotorPosController* motor_wheeldir_posctrl[4]   = {nullptr};
MotorVelController* motor_wheeldir_velctrl[4]   = {nullptr};
// 初始化一个底盘
static void Motion_Init()
{
    for (size_t i = 0; i < 4; i++)
    {
        motor_wheeldir_posctrl[i] =
                new MotorPosController(Device::motor::motor_wheel_dir[i],
                                       {
                                               .position_pid       = motor_wheeldir_pospid,
                                               .velocity_pid       = motor_wheeldir_velpid,
                                               .pos_vel_freq_ratio = 10,
                                       });
        motor_wheeldir_velctrl[i]   = new MotorVelController(Device::motor::motor_wheel_dir[i],
                                                             {.pid = motor_wheeldir_velpid});
        motor_wheelspeed_velctrl[i] = new MotorVelController(Device::motor::motor_wheel_speed[i],
                                                             {.ctrl_mode = ControlMode::InternalVel,
                                                              .internal_set_ratio = 50});
    }
#warning ("现在底盘的光电门还没装上去，自动校准暂不开启")
    chassis_ = new Special_Steering4(Special_Steering4::Config{
            .enable_calibration = false,
            .radius             = 45.0f,
            .distance_x         = 226.24f,
            .distance_y         = 226.24f,
            .wheel_front_right =
                    {
                            .cfg =
                                    {
                                            .drive_motor  = motor_wheelspeed_velctrl[0],
                                            .steer_motor  = motor_wheeldir_posctrl[0],
                                            .steer_offset = 0.0f,

                                    },
                            .calib_cfg =
                                    {
                                            .steer_motor = motor_wheeldir_velctrl[0],
                                            .photogate   = GPIO_FRONT_RIGHT,
                                    },

                    },
            .wheel_front_left =
                    {
                            .cfg =
                                    {
                                            .drive_motor  = motor_wheelspeed_velctrl[1],
                                            .steer_motor  = motor_wheeldir_posctrl[1],
                                            .steer_offset = 0.0f,
                                    },
                            .calib_cfg =
                                    {
                                            .steer_motor = motor_wheeldir_velctrl[1],
                                            .photogate   = GPIO_FRONT_LEFT,
                                    },
                    },
            .wheel_rear_left =
                    {
                            .cfg =
                                    {
                                            .drive_motor  = motor_wheelspeed_velctrl[2],
                                            .steer_motor  = motor_wheeldir_posctrl[2],
                                            .steer_offset = 0.0f,
                                    },
                            .calib_cfg =
                                    {
                                            .steer_motor = motor_wheeldir_velctrl[2],
                                            .photogate   = GPIO_REAR_LEFT,
                                    },
                    },
            .wheel_rear_right =
                    {
                            .cfg =
                                    {
                                            .drive_motor  = motor_wheelspeed_velctrl[3],
                                            .steer_motor  = motor_wheeldir_posctrl[3],
                                            .steer_offset = 0.0f,
                                    },
                            .calib_cfg =
                                    {
                                            .steer_motor = motor_wheeldir_velctrl[3],
                                            .photogate   = GPIO_REAR_RIGHT,
                                    },
                    },

    });
}

static void Loc_Init()
{
    chassis_loc_ = new JustEncoder(*chassis_);
}

static void Controller_Init()
{
    chassis_ctrl_ =
            new Master(*chassis_,
                       *chassis_loc_,
                       {.posture_error_pd_cfg =
                                {
                                        .vx = {.Kp = 5.0f, .Kd = 3.0f, .abs_output_max = 0.1f},
                                        .vy = {.Kp = 5.0f, .Kd = 3.0f, .abs_output_max = 0.1f},
                                        .wz = {.Kp = 30.0f, .Kd = 4.0f, .abs_output_max = 25.0f},
                                },
                        .limit = {.x   = {.max_spd = 1.0f, .max_acc = 1.2f, .max_jerk = 20.0f},
                                  .y   = {.max_spd = 1.0f, .max_acc = 1.2f, .max_jerk = 20.0f},
                                  .yaw = {.max_spd = 90.0f, .max_acc = 45.0f, .max_jerk = 90.0f}}});
}

void APP_CHASSIS_Init()
{
    Motion_Init(); // 底盘启动
}
void Ctrl_Init()
{
    Loc_Init();        // 定位启动
    Controller_Init(); // 底盘控制器启动
}
void update_1kHz()
{
    if (chassis_loc_)
        chassis_loc_->update(0.001f);
    if (chassis_ctrl_)
        chassis_ctrl_->controllerUpdate();

    chassis_->update();
}

void update_100Hz() {}

} // namespace Chassis
