#pragma once

#include <stdint.h>

#include "motor/motor.h"

namespace motor {

/**
 * @brief 框架管理的电机轴
 */
enum class Axis : uint8_t {
    /** X 轴 */
    X,
    /** Y 轴 */
    Y,
    /** Z 轴 */
    Z,
    /** 旋转轴 */
    Rotation,
};

/**
 * @brief 获取按硬件配置组装的轴电机
 * @param axis 目标轴
 * @return 统一电机接口
 */
Motor& systemMotor(Axis axis);

}
