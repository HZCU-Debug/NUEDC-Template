#pragma once

#include <stdint.h>

namespace io {

/**
 * @brief 提供超时和延时所需的单调时钟
 */
class Clock {
public:
    /**
     * @brief 销毁时钟
     */
    virtual ~Clock() = default;

    /**
     * @brief 获取启动后的毫秒数
     * @return 单调递增的毫秒数，允许自然回绕
     */
    virtual uint32_t nowMs() const = 0;

    /**
     * @brief 阻塞指定毫秒数
     * @param durationMs 延时时长
     */
    virtual void delayMs(uint32_t durationMs) = 0;

};

}
