#include "program/motor_test.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "config/parameters.h"
#include "ui/view.h"

extern HardwareSerial Serial;

namespace program {
namespace {

int16_t decodeInt16(const uint8_t* data) {
    const uint16_t encoded =
        static_cast<uint16_t>(static_cast<uint16_t>(data[0]) << 8) | data[1];
    return encoded <= INT16_MAX
               ? static_cast<int16_t>(encoded)
               : static_cast<int16_t>(static_cast<int32_t>(encoded) - 0x10000L);
}

float decodeFloat(const uint8_t* data) {
    const uint32_t encoded = static_cast<uint32_t>(data[0]) << 24 |
                             static_cast<uint32_t>(data[1]) << 16 |
                             static_cast<uint32_t>(data[2]) << 8 | data[3];
    float value;
    memcpy(&value, &encoded, sizeof(value));
    return value;
}

}

MotorTestProgram::MotorTestProgram(
    HardwareSerial& serial, motor::Motor& x, motor::Motor& y,
    motor::Motor& z, motor::Motor& rotation, const comm::LinkConfig& config)
    : link_(serial, config),
      motors_{&x, &y, &z, &rotation},
      state_(State::Idle),
      axis_('-'),
      rpm_(0),
      degrees_(0.0f),
      positionMode_(false) {}

void MotorTestProgram::start(Adafruit_GFX& display, runtime::SystemState&) {
    ui::view::beginPage(display, "Test");
    axis_ = '-';
    rpm_ = 0;
    degrees_ = 0.0f;
    positionMode_ = false;
    if (!link_.begin()) {
        state_ = State::Error;
        render(display, "Start failed");
        return;
    }
    for (size_t index = 0; index < 4; ++index) {
        if (!motors_[index]->begin()) {
            state_ = State::Error;
            render(display, "Motor error");
            return;
        }
    }
    state_ = State::Ready;
    render(display, "Ready");
}

void MotorTestProgram::update(Adafruit_GFX& display, runtime::SystemState&,
                              ui::Event) {
    if (state_ != State::Ready) return;
    const comm::Event event = link_.poll();
    if (event.type != comm::EventType::Message) return;
    const bool accepted =
        applySpeed(event.message) || applyPosition(event.message);
    render(display, accepted ? (positionMode_ ? "Moving" : "Running")
                             : "Bad command");
}

void MotorTestProgram::requestExit() {
    if (state_ == State::Idle) return;
    link_.cancel();
    for (size_t index = 0; index < 4; ++index) {
        motors_[index]->stop();
    }
    state_ = State::Idle;
}

bool MotorTestProgram::readyToExit() const { return state_ == State::Idle; }

void MotorTestProgram::stop(runtime::SystemState&) { requestExit(); }

bool MotorTestProgram::decodeAxis(uint8_t value, motor::Axis& axis) {
    switch (value) {
        case 'X':
            axis = motor::Axis::X;
            return true;
        case 'Y':
            axis = motor::Axis::Y;
            return true;
        case 'Z':
            axis = motor::Axis::Z;
            return true;
        case 'R':
            axis = motor::Axis::Rotation;
            return true;
        default:
            return false;
    }
}

motor::Motor& MotorTestProgram::motorFor(motor::Axis axis) {
    switch (axis) {
        case motor::Axis::X:
            return *motors_[0];
        case motor::Axis::Y:
            return *motors_[1];
        case motor::Axis::Z:
            return *motors_[2];
        case motor::Axis::Rotation:
            return *motors_[3];
    }
    return *motors_[0];
}

bool MotorTestProgram::applySpeed(const comm::MessageView& message) {
    if (message.type != kMotorSpeedMessage ||
        message.delivery != comm::Delivery::Reliable || message.size != 3) {
        return false;
    }
    motor::Axis axis;
    const int16_t rpm = decodeInt16(message.payload + 1);
    if (!decodeAxis(message.payload[0], axis) ||
        rpm < -config::kMotorTestMaximumRpm ||
        rpm > config::kMotorTestMaximumRpm) {
        return false;
    }
    motor::Motor& selected = motorFor(axis);
    if (!selected.enable() ||
        !(rpm == 0 ? selected.stop()
                   : selected.run(rpm, config::kMotorTestAcceleration))) {
        return false;
    }
    axis_ = static_cast<char>(message.payload[0]);
    rpm_ = rpm;
    positionMode_ = false;
    return true;
}

bool MotorTestProgram::applyPosition(const comm::MessageView& message) {
    if (message.type != kMotorPositionMessage ||
        message.delivery != comm::Delivery::Reliable || message.size != 7) {
        return false;
    }
    motor::Axis axis;
    const float degrees = decodeFloat(message.payload + 1);
    const uint16_t rpm = static_cast<uint16_t>(message.payload[5]) << 8 |
                         message.payload[6];
    if (!decodeAxis(message.payload[0], axis) || !isfinite(degrees) ||
        rpm == 0 ||
        rpm > static_cast<uint16_t>(config::kMotorTestMaximumRpm)) {
        return false;
    }
    motor::Motor& selected = motorFor(axis);
    if (!selected.enable() ||
        !selected.moveAbsolute(
            degrees,
            motor::MotionOptions(rpm, config::kMotorTestAcceleration))) {
        return false;
    }
    axis_ = static_cast<char>(message.payload[0]);
    rpm_ = static_cast<int16_t>(rpm);
    degrees_ = degrees;
    positionMode_ = true;
    return true;
}

void MotorTestProgram::render(Adafruit_GFX& display,
                              const char* status) const {
    ui::view::beginBody(display);
    display.setCursor(6, 40);
    display.print(status);
    char command[24];
    if (positionMode_) {
        snprintf(command, sizeof(command), "%c:%.2f deg", axis_, degrees_);
    } else {
        snprintf(command, sizeof(command), "%c:%d RPM", axis_, rpm_);
    }
    display.setCursor(6, 70);
    display.print(command);
    display.setCursor(6, 106);
    display.print("S4 Back");
}

runtime::Program& motorTest() {
    static MotorTestProgram program(
        Serial, motor::systemMotor(motor::Axis::X),
        motor::systemMotor(motor::Axis::Y),
        motor::systemMotor(motor::Axis::Z),
        motor::systemMotor(motor::Axis::Rotation));
    return program;
}

}
