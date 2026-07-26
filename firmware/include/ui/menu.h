#pragma once

#include <Adafruit_GFX.h>
#include <stddef.h>
#include <stdint.h>

namespace ui {

/**
 * @brief 菜单输入事件
 */
enum class Event : uint8_t {
    /** 没有输入 */
    None,
    /** 选择上一项 */
    Up,
    /** 选择下一项 */
    Down,
    /** 进入当前项 */
    Select,
    /** 退出当前项 */
    Back,
};

/**
 * @brief 可由菜单选择和运行的页面
 */
class Item {
public:
    /**
     * @brief 创建菜单项
     * @param label 菜单显示名称，生命周期必须长于 Item
     */
    explicit Item(const char* label);

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
     * @brief 初始化长期状态
     */
    virtual void setup() {}

    /**
     * @brief 进入页面并完成首次绘制
     * @param display 图形屏幕
     */
    virtual void enter(Adafruit_GFX&) {}

    /**
     * @brief 运行页面逻辑并处理输入
     * @param display 图形屏幕
     * @param event 本轮输入事件
     */
    virtual void loop(Adafruit_GFX& display, Event event) = 0;

    /**
     * @brief 退出页面并停止业务活动
     */
    virtual void exit() {}

private:
    const char* label_;
};

/**
 * @brief 管理单级菜单导航、渲染和 Item 生命周期
 */
class Menu {
public:
    /**
     * @brief 绑定屏幕和静态菜单项
     * @param display 已初始化的图形屏幕
     * @param title 菜单标题，生命周期必须长于 Menu
     * @param items 非拥有型 Item 指针数组，生命周期必须长于 Menu
     * @param itemCount 菜单项数量
     */
    Menu(Adafruit_GFX& display, const char* title, Item* const* items,
         size_t itemCount);

    /**
     * @brief 初始化所有 Item 并绘制菜单
     * @return 配置有效时返回 true
     */
    bool begin();

    /**
     * @brief 处理输入并运行当前 Item
     * @param event 本轮输入事件
     */
    void loop(Event event = Event::None);

private:
    void moveSelection(int8_t offset);
    void renderMenu();

    Adafruit_GFX& display_;
    const char* title_;
    Item* const* items_;
    size_t itemCount_;
    size_t selectedIndex_;
    size_t firstVisibleIndex_;
    Item* activeItem_;
    bool begun_;
};

}
