#pragma once

#include <stdint.h>

#include "config/hardware.h"

namespace config {

/** 按钮消抖时间，单位 ms */
constexpr uint32_t kButtonDebounceMs = 25;

/** 电机测试程序加速度档位 */
constexpr uint8_t kMotorTestAcceleration =
    kMotorModel == MotorModel::Zdt ? 100 : 10;

/** 电机测试程序最大转速，单位 RPM */
constexpr int16_t kMotorTestMaximumRpm = 6000;

}
