#pragma once

#include <stddef.h>
#include <stdint.h>

#include "ui/display.h"
#include "ui/event.h"

namespace runtime {
class Program;
class ProgramRunner;
}

namespace ui {

/**
 * @brief 由菜单显示并启动一个 Program 的入口
 */
class Item {
public:
    /**
     * @brief 创建菜单项
     * @param label 菜单显示名称
     * @param runner Program 调度器
     * @param program 菜单进入时启动的 Program
     */
    Item(const char* label, runtime::ProgramRunner& runner,
         runtime::Program& program);

    /**
     * @brief 销毁菜单项
     */
    virtual ~Item() = default;

    /**
     * @brief 获取菜单显示名称
     * @return 菜单显示名称
     */
    const char* label() const;

    /**
     * @brief 启动绑定的 Program
     * @param display 图形屏幕
     */
    virtual void enter(Display& display);

    /**
     * @brief 更新当前 Program
     * @param display 图形屏幕
     * @param event 本轮输入事件
     */
    void loop(Display& display, Event event);

    /**
     * @brief 停止当前 Program
     */
    void exit();

protected:
    runtime::ProgramRunner& runner_;
    runtime::Program& program_;

private:
    const char* label_;
};

/**
 * @brief 启动前传入强类型配置的菜单项
 * @tparam ProgramType 提供 configure 方法的 Program 类型
 * @tparam Configuration Program 使用的配置类型
 */
template <typename ProgramType, typename Configuration>
class ConfiguredItem final : public Item {
public:
    /**
     * @brief 绑定菜单名称、Program 和启动配置
     * @param label 菜单显示名称
     * @param runner Program 调度器
     * @param program 菜单进入时启动的 Program
     * @param configuration 启动前传入 Program 的配置
     */
    ConfiguredItem(const char* label, runtime::ProgramRunner& runner,
                   ProgramType& program,
                   const Configuration& configuration)
        : Item(label, runner, program),
          configuredProgram_(program),
          configuration_(configuration) {}

    /**
     * @brief 配置并启动绑定的 Program
     * @param display 图形屏幕
     */
    void enter(Display& display) override {
        configuredProgram_.configure(configuration_);
        Item::enter(display);
    }

private:
    ProgramType& configuredProgram_;
    Configuration configuration_;
};

/**
 * @brief 管理单级菜单导航、渲染和 Program 入口
 */
class Menu {
public:
    /**
     * @brief 绑定屏幕和静态菜单项
     * @param display 已初始化屏幕
     * @param title 菜单标题
     * @param items 菜单项指针数组
     * @param itemCount 菜单项数量
     */
    Menu(Display& display, const char* title, Item* const* items,
         size_t itemCount);

    /**
     * @brief 验证配置并绘制菜单
     * @return 配置有效时返回 true
     */
    bool begin();

    /**
     * @brief 处理输入并运行当前菜单项
     * @param event 本轮输入事件
     */
    void loop(Event event = Event::None);

private:
    void moveSelection(int8_t offset);
    void renderMenu();

    Display& display_;
    const char* title_;
    Item* const* items_;
    size_t itemCount_;
    size_t selectedIndex_;
    size_t firstVisibleIndex_;
    Item* activeItem_;
    bool begun_;
};

}
