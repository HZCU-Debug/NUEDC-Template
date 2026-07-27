#pragma once

#include "ui/display.h"

namespace ui {
namespace view {

const uint16_t kBackgroundColor = 0x0000;
const uint16_t kTextColor = 0xFFFF;
const uint16_t kTitleColor = 0x07FF;

/**
 * @brief 清屏并绘制页面标题
 * @param display 图形屏幕
 * @param title 页面标题
 */
inline void beginPage(Display& display, const char* title) {
    display.clear(kBackgroundColor);
    display.text(4, 4, title, kTitleColor, kBackgroundColor);
}

/**
 * @brief 擦除页面正文区域
 * @param display 图形屏幕
 */
inline void beginBody(Display& display) {
    display.text(4, 36, "                            ", kTextColor,
                 kBackgroundColor);
    display.text(4, 58, "                            ", kTextColor,
                 kBackgroundColor);
    display.text(4, 80, "                            ", kTextColor,
                 kBackgroundColor);
}

}
}
