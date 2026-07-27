#pragma once

#include "ui/display.h"

namespace platform {

/**
 * @brief 使用逐飞 IPS114 驱动实现菜单屏幕
 */
class Ips114Display final : public ui::Display {
public:
    /**
     * @brief 初始化横向 IPS114 屏幕
     * @return 初始化完成后返回 true
     */
    bool begin() override;

    /**
     * @brief 使用指定颜色清屏
     * @param color RGB565 颜色
     */
    void clear(uint16_t color) override;

    /**
     * @brief 显示字符串
     * @param x 横坐标
     * @param y 纵坐标
     * @param value 字符串
     * @param foreground 前景色
     * @param background 背景色
     */
    void text(uint16_t x, uint16_t y, const char* value,
              uint16_t foreground, uint16_t background) override;

    /**
     * @brief 显示有符号整数
     * @param x 横坐标
     * @param y 纵坐标
     * @param value 数值
     * @param foreground 前景色
     * @param background 背景色
     */
    void integer(uint16_t x, uint16_t y, int32_t value, uint16_t foreground,
                 uint16_t background) override;

    /**
     * @brief 显示两位小数
     * @param x 横坐标
     * @param y 纵坐标
     * @param value 数值
     * @param foreground 前景色
     * @param background 背景色
     */
    void decimal(uint16_t x, uint16_t y, float value, uint16_t foreground,
                 uint16_t background) override;

    /**
     * @brief 获取横屏宽度
     * @return 240
     */
    uint16_t width() const override;

    /**
     * @brief 获取横屏高度
     * @return 135
     */
    uint16_t height() const override;
};

}
