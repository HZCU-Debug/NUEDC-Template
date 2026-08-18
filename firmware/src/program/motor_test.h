#pragma once

#include <Arduino.h>

#include "comm/link.h"
#include "motor/system.h"
#include "runtime/program.h"

namespace program {

/** 单轴速度命令消息类型 */
const uint8_t kMotorSpeedMessage = 0x01;

/** 单轴绝对位置命令消息类型 */
const uint8_t kMotorPositionMessage = 0x02;

/**
 * @brief 接收上位机单轴速度和绝对位置命令
 */
class MotorTestProgram final : public runtime::Program {
public:
    /**
     * @brief 绑定上位机串口和四个轴电机
     * @param serial 上位机通信串口
     * @param x X 轴电机
     * @param y Y 轴电机
     * @param z Z 轴电机
     * @param rotation 旋转轴电机
     * @param config 串口链路配置
     */
    MotorTestProgram(
        HardwareSerial& serial, motor::Motor& x, motor::Motor& y,
        motor::Motor& z, motor::Motor& rotation,
        const comm::LinkConfig& config = comm::LinkConfig());

    /** @copydoc runtime::Program::start */
    void start(Adafruit_GFX& display, runtime::SystemState& state) override;

    /** @copydoc runtime::Program::update */
    void update(Adafruit_GFX& display, runtime::SystemState& state,
                ui::Event event) override;

    /** @copydoc runtime::Program::requestExit */
    void requestExit() override;

    /** @copydoc runtime::Program::readyToExit */
    bool readyToExit() const override;

    /** @copydoc runtime::Program::stop */
    void stop(runtime::SystemState& state) override;

private:
    enum class State : uint8_t {
        Ready,
        Idle,
        Error,
    };

    static bool decodeAxis(uint8_t value, motor::Axis& axis);
    motor::Motor& motorFor(motor::Axis axis);
    bool applySpeed(const comm::MessageView& message);
    bool applyPosition(const comm::MessageView& message);
    void render(Adafruit_GFX& display, const char* status) const;

    comm::Link<7> link_;
    motor::Motor* motors_[4];
    State state_;
    char axis_;
    int16_t rpm_;
    float degrees_;
    bool positionMode_;
};

}
