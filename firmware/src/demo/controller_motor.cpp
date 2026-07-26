/**
 * @file controller_motor.cpp
 * @brief 接收 Python 手柄发送的速度消息并驱动电机持续旋转
 */
#include "demo.h"

#include <Arduino.h>

#include "comm/link.h"
#include "zdt/motor.h"

namespace demo {
namespace controllerMotor {
namespace {

const uint32_t kSerialBaudRate = 115200;
const int8_t kMotorRxPin = 25;
const int8_t kMotorTxPin = 26;
const uint8_t kMotorAddress = 1;
const uint32_t kPulsesPerRevolution = 3200;
const uint16_t kMaximumMotorRpm = 300;
const uint8_t kAcceleration = 0;
const uint32_t kCommandTimeoutMs = 500;
const uint8_t kVelocityMessage = 1;

zdt::Bus motorBus(
    Serial2, zdt::BusConfig(kSerialBaudRate, kMotorRxPin, kMotorTxPin));
zdt::Motor motor(
    motorBus, zdt::MotorConfig(kMotorAddress, kPulsesPerRevolution));
comm::Link<2> controllerLink(Serial, comm::LinkConfig(kSerialBaudRate));

bool motorReady = false;
bool motorRunning = false;
uint32_t lastCommandAt = 0;

bool decodeVelocity(const uint8_t* payload, size_t size, int16_t& velocity) {
    if (payload == nullptr || size != 2) {
        return false;
    }

    const uint16_t encoded =
        static_cast<uint16_t>(static_cast<uint16_t>(payload[0]) << 8) | payload[1];
    const int32_t decoded =
        encoded <= INT16_MAX ? encoded : static_cast<int32_t>(encoded) - 0x10000L;
    if (decoded < -1000 || decoded > 1000) {
        return false;
    }

    velocity = static_cast<int16_t>(decoded);
    return true;
}

void applyCommand(const comm::MessageView& message) {
    int16_t velocity = 0;
    if (!motorReady || message.type != kVelocityMessage ||
        !decodeVelocity(message.payload, message.size, velocity)) {
        return;
    }

    const int16_t rpm = static_cast<int16_t>(
        static_cast<int32_t>(velocity) * kMaximumMotorRpm / 1000);
    const zdt::Status status =
        rpm == 0 ? motor.stop() : motor.run(rpm, kAcceleration);
    if (status) {
        motorRunning = rpm != 0;
        lastCommandAt = millis();
    }
}

void readCommands() {
    for (;;) {
        const comm::Event event = controllerLink.poll();
        if (event.type == comm::EventType::None) {
            return;
        }
        if (event.type == comm::EventType::Message) {
            applyCommand(event.message);
        }
    }
}

}

void setup() {
    controllerLink.begin();
    zdt::Status status = motorBus.begin();
    if (status) {
        delay(2000);
        status = motor.enable();
    }
    if (status) {
        const zdt::Result<float> position = motor.readPositionDegrees();
        status = zdt::Status(position.error);
    }
    motorReady = static_cast<bool>(status);
    lastCommandAt = millis();
}

void loop() {
    readCommands();
    if (motorRunning && millis() - lastCommandAt >= kCommandTimeoutMs) {
        motor.stop();
        motorRunning = false;
    }
}

}

}
