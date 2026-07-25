/**
 * @file demo.h
 * @brief 所有固件 Demo 的统一入口声明
 */
#pragma once

namespace demo {

/**
 * @brief 手柄控制电机 Demo
 */
namespace controllerMotor {
void setup();
void loop();
}

/**
 * @brief 电机速度梯度 Demo
 */
namespace motorRamp {
void setup();
void loop();
}

/**
 * @brief 电机位置读取 Demo
 */
namespace motorPosition {
void setup();
void loop();
}

/**
 * @brief 非可靠消息发送 Demo
 */
namespace commUnreliable {
void setup();
void loop();
}

/**
 * @brief 可靠消息发送 Demo
 */
namespace commReliable {
void setup();
void loop();
}

}
