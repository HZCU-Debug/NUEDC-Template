/**
 * @file motor_position.cpp
 * @brief 失能电机并每 500 ms 在 USB 串口输出一次转子角度
 */
#include "demo.h"

#include <Arduino.h>

#include "zdt/motor.h"

namespace demo {
namespace motorPosition {
namespace {

const uint32_t kSerialBaudRate = 115200;
const int8_t kMotorRxPin = 25;
const int8_t kMotorTxPin = 26;
const uint8_t kMotorAddress = 1;
const uint32_t kPulsesPerRevolution = 3200;
const uint32_t kReadIntervalMs = 500;

zdt::Bus motorBus(
    Serial2, zdt::BusConfig(kSerialBaudRate, kMotorRxPin, kMotorTxPin));
zdt::Motor motor(
    motorBus, zdt::MotorConfig(kMotorAddress, kPulsesPerRevolution));

bool motorReady = false;
uint32_t lastReadAt = 0;

}

void setup() {
    Serial.begin(kSerialBaudRate);
    zdt::Status status = motorBus.begin();
    if (status) {
        delay(2000);
        status = motor.disable();
    }
    motorReady = static_cast<bool>(status);
    if (!motorReady) {
        Serial.printf("disable_error=%u\n", static_cast<unsigned>(status.error));
    }
    lastReadAt = millis();
}

void loop() {
    const uint32_t now = millis();
    if (!motorReady || now - lastReadAt < kReadIntervalMs) {
        return;
    }

    const zdt::Result<float> position = motor.readPositionDegrees();
    if (position) {
        Serial.printf("%.2f\n", position.value);
    } else {
        Serial.printf("position_error=%u\n", static_cast<unsigned>(position.error));
    }
    lastReadAt = now;
}

}

}
