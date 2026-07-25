/**
 * @file motor_ramp.cpp
 * @brief 驱动电机以 10 RPM 为步长在 -300 至 300 RPM 之间每 100 ms 往返变化
 */
#include "demo.h"

#include <Arduino.h>

#include "zdt/motor.h"

namespace demo {
namespace motorRamp {
namespace {

const uint32_t kSerialBaudRate = 115200;
const int8_t kMotorRxPin = 25;
const int8_t kMotorTxPin = 26;
const uint8_t kMotorAddress = 1;
const uint32_t kPulsesPerRevolution = 3200;
const uint32_t kStepIntervalMs = 100;
const int16_t kSpeedStepRpm = 10;
const int16_t kMaximumRpm = 300;

zdt::Bus motorBus(
    Serial2, zdt::BusConfig(kSerialBaudRate, kMotorRxPin, kMotorTxPin));
zdt::Motor motor(
    motorBus, zdt::MotorConfig(kMotorAddress, kPulsesPerRevolution));

bool motorReady = false;
int16_t targetRpm = 0;
int8_t direction = 1;
uint32_t lastStepAt = 0;

}

void setup() {
    zdt::Status status = motorBus.begin();
    if (status) {
        delay(2000);
        status = motor.enable();
    }
    motorReady = static_cast<bool>(status);
    lastStepAt = millis();
}

void loop() {
    const uint32_t now = millis();
    if (!motorReady || now - lastStepAt < kStepIntervalMs) {
        return;
    }

    targetRpm = static_cast<int16_t>(targetRpm + direction * kSpeedStepRpm);
    if (targetRpm == kMaximumRpm) {
        direction = -1;
    } else if (targetRpm == -kMaximumRpm) {
        direction = 1;
    }

    if (targetRpm == 0) {
        motor.stop();
    } else {
        motor.run(targetRpm);
    }
    lastStepAt = now;
}

}

}
