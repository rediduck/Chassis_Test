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
JustEncoder*        chassis_loc_;
Master*             chassis_ctrl_;

// 初始化一个底盘
static void Motion_Init()
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
            .enable_calibration = false,
            .radius             = 100.0f,
            .distance_x         = 63.0f,
            .distance_y         = 200.0f,
            .wheel_front_right =
                    {
                            .cfg =
                                    {
                                            .drive_motor  = motor_wheelspeed_velctrl[0],
                                            .steer_motor  = motor_wheeldir_posctrl[0],
                                            .steer_offset = -90.0f,
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
                                            .steer_offset = 90.0f,
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
                        .limit = {
                                .x   = {.max_vel = 1.0f, .max_accel = 1.2f, .max_jerk = 20.0f},
                                .y   = {.max_vel = 1.0f, .max_accel = 1.2f, .max_jerk = 20.0f},
                                .yaw = {.max_vel = 90.0f, .max_accel = 45.0f, .max_jerk = 90.0f},
                        }});
}

void APP_CHASSIS_Init()
{
    Motion_Init();     // 底盘启动
    Loc_Init();        // 定位启动
    Controller_Init(); // 底盘控制器启动
}
