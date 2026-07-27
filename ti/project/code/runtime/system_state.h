#pragma once

#include <stdint.h>

namespace runtime {

/**
 * @brief 跨 Program 共享的系统状态
 */
struct SystemState {
    SystemState() : now(0) {}

    uint32_t now;
};

}
