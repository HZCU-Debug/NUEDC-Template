#pragma once

#include "runtime/system_state.h"
#include "ui/display.h"
#include "ui/event.h"

namespace runtime {

/**
 * @brief 可独占运行并绘制活动页面的程序
 */
class Program {
public:
    /**
     * @brief 销毁程序
     */
    virtual ~Program() = default;

    /**
     * @brief 启动程序并初始化本轮运行上下文
     * @param display 图形屏幕
     * @param state 共享系统状态
     */
    virtual void start(ui::Display& display, SystemState& state) = 0;

    /**
     * @brief 更新程序逻辑和活动页面
     * @param display 图形屏幕
     * @param state 共享系统状态
     * @param event 本轮输入事件
     */
    virtual void update(ui::Display& display, SystemState& state,
                        ui::Event event) = 0;

    /**
     * @brief 停止程序并结束业务活动
     * @param state 共享系统状态
     */
    virtual void stop(SystemState&) {}
};

}
