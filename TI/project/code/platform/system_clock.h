#pragma once

#include "io/clock.h"
namespace platform {

/**
 * @brief 使用逐飞阻塞延时累计系统毫秒时钟
 */
class SystemClock final : public io::Clock {
public:
    /**
     * @brief 创建未启动的系统时钟
     */
    SystemClock();

    /**
     * @brief 重置累计毫秒数
     */
    void begin();

    /**
     * @brief 获取启动后的毫秒数
     * @return 单调递增的毫秒数，允许自然回绕
     */
    uint32_t nowMs() const override;

    /**
     * @brief 使用逐飞延时等待指定毫秒数
     * @param durationMs 延时时长
     */
    void delayMs(uint32_t durationMs) override;

private:
    uint32_t nowMs_;
};

}
