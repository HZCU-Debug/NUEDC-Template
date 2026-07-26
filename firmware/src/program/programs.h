/**
 * @file programs.h
 * @brief 固件菜单使用的 Program 声明
 */
#pragma once

#include "runtime/program.h"

namespace program {

/**
 * @brief 手柄控制电机 Demo
 * @return Demo Program
 */
runtime::Program& controllerMotor();

/**
 * @brief 电机速度梯度 Demo
 * @return Demo Program
 */
runtime::Program& motorRamp();

/**
 * @brief 电机位置读取 Demo
 * @return Demo Program
 */
runtime::Program& motorPosition();

/**
 * @brief 非可靠消息发送 Demo
 * @return Demo Program
 */
runtime::Program& commUnreliable();

/**
 * @brief 可靠消息发送 Demo
 * @return Demo Program
 */
runtime::Program& commReliable();

}
