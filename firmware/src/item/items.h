/**
 * @file items.h
 * @brief 固件菜单使用的 Item 声明
 */
#pragma once

#include "ui/menu.h"

namespace item {

/**
 * @brief 手柄控制电机 Demo
 * @return Demo 菜单项
 */
ui::Item& controllerMotor();

/**
 * @brief 电机速度梯度 Demo
 * @return Demo 菜单项
 */
ui::Item& motorRamp();

/**
 * @brief 电机位置读取 Demo
 * @return Demo 菜单项
 */
ui::Item& motorPosition();

/**
 * @brief 非可靠消息发送 Demo
 * @return Demo 菜单项
 */
ui::Item& commUnreliable();

/**
 * @brief 可靠消息发送 Demo
 * @return Demo 菜单项
 */
ui::Item& commReliable();

}
