#pragma once

#include <stdint.h>

namespace ui {

/**
 * @brief 提供菜单和 Program 所需的屏幕绘制能力
 */
class Display {
public:
    /**
     * @brief 销毁屏幕
     */
    virtual ~Display() = default;

    /**
     * @brief 初始化屏幕
     * @return 初始化是否成功
     */
    virtual bool begin() = 0;

    /**
     * @brief 使用指定颜色清屏
     * @param color RGB565 颜色
     */
    virtual void clear(uint16_t color) = 0;

    /**
     * @brief 显示字符串
     * @param x 横坐标
     * @param y 纵坐标
     * @param value 字符串
     * @param foreground 前景色
     * @param background 背景色
     */
    virtual void text(uint16_t x, uint16_t y, const char* value,
                      uint16_t foreground, uint16_t background) = 0;

    /**
     * @brief 显示有符号整数
     * @param x 横坐标
     * @param y 纵坐标
     * @param value 数值
     * @param foreground 前景色
     * @param background 背景色
     */
    virtual void integer(uint16_t x, uint16_t y, int32_t value,
                         uint16_t foreground, uint16_t background) = 0;

    /**
     * @brief 显示两位小数
     * @param x 横坐标
     * @param y 纵坐标
     * @param value 数值
     * @param foreground 前景色
     * @param background 背景色
     */
    virtual void decimal(uint16_t x, uint16_t y, float value,
                         uint16_t foreground, uint16_t background) = 0;

    /**
     * @brief 获取屏幕宽度
     * @return 横向像素数
     */
    virtual uint16_t width() const = 0;

    /**
     * @brief 获取屏幕高度
     * @return 纵向像素数
     */
    virtual uint16_t height() const = 0;
};

}
