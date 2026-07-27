#pragma once

#include "runtime/program.h"

namespace program {

/**
 * @brief 获取手柄控制电机 Demo
 * @return Demo Program
 */
runtime::Program& controllerMotor();

/**
 * @brief 获取电机速度梯度 Demo
 * @return Demo Program
 */
runtime::Program& motorRamp();

/**
 * @brief 获取电机位置读取 Demo
 * @return Demo Program
 */
runtime::Program& motorPosition();

/**
 * @brief 获取非可靠消息发送 Demo
 * @return Demo Program
 */
runtime::Program& commUnreliable();

/**
 * @brief 获取可靠消息发送 Demo
 * @return Demo Program
 */
runtime::Program& commReliable();

}
