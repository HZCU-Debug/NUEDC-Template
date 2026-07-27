#pragma once

#include <stdint.h>

namespace ui {

/**
 * @brief 菜单输入事件
 */
enum class Event : uint8_t {
    None,
    Up,
    Down,
    Select,
    Back,
};

}
