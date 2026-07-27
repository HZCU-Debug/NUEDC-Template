#pragma once

#include "runtime/program.h"
#include "runtime/state_machine.h"

namespace program {

/**
 * @brief 状态机示例使用的状态
 */
enum class DemoState {
    EntryA,
    EntryB,
    Shared,
    Loop,
};

/**
 * @brief 状态机示例启动配置
 */
struct DemoConfiguration {
    /**
     * @brief 创建启动配置
     * @param entry 状态机入口
     */
    explicit DemoConfiguration(DemoState entry) : entry(entry) {}

    DemoState entry;
};

/**
 * @brief 展示多入口和循环跳转的 Program
 */
class StateMachineDemoProgram final : public runtime::Program {
public:
    /**
     * @brief 绑定共享状态机
     * @param stateMachine 状态机实例
     */
    explicit StateMachineDemoProgram(
        runtime::StateMachine<DemoState>& stateMachine);

    /**
     * @brief 设置下次启动入口
     * @param configuration 启动配置
     */
    void configure(const DemoConfiguration& configuration);

    /**
     * @brief 从配置入口启动示例
     * @param display 图形屏幕
     * @param state 共享系统状态
     */
    void start(ui::Display& display, runtime::SystemState& state) override;

    /**
     * @brief 处理当前状态并完成一次跳转
     * @param display 图形屏幕
     * @param state 共享系统状态
     * @param event 本轮输入事件
     */
    void update(ui::Display& display, runtime::SystemState& state,
                ui::Event event) override;

private:
    void render(ui::Display& display) const;

    runtime::StateMachine<DemoState>& stateMachine_;
    DemoConfiguration configuration_;
};

}
