#include <Arduino.h>

#include "demo/velocity_command.h"
#include "zdt/motor.h"

namespace {

const uint32_t kSerialBaudRate = 115200;
const int8_t kMotorRxPin = 25;
const int8_t kMotorTxPin = 26;
const uint8_t kMotorAddress = 1;
const uint32_t kPulsesPerRevolution = 3200;
const uint16_t kMaximumMotorRpm = 300;
const uint8_t kAcceleration = 10;
const uint32_t kCommandTimeoutMs = 500;
const size_t kCommandBufferSize = 16;

zdt::Bus motorBus(
    Serial2, zdt::BusConfig(kSerialBaudRate, kMotorRxPin, kMotorTxPin));
zdt::Motor motor(
    motorBus, zdt::MotorConfig(kMotorAddress, kPulsesPerRevolution));

char commandBuffer[kCommandBufferSize];
size_t commandLength = 0;
bool discardingCommand = false;
bool motorReady = false;
bool motorRunning = false;
uint32_t lastCommandAt = 0;

void applyCommand() {
    commandBuffer[commandLength] = '\0';

    int16_t velocity = 0;
    if (!demo::parseVelocityCommand(commandBuffer, velocity) || !motorReady) {
        return;
    }

    const int16_t rpm = demo::velocityToRpm(velocity, kMaximumMotorRpm);
    const zdt::Status status =
        rpm == 0 ? motor.stop() : motor.run(rpm, kAcceleration);
    if (status) {
        motorRunning = rpm != 0;
        lastCommandAt = millis();
    }
}

void readCommands() {
    while (Serial.available()) {
        const char received = static_cast<char>(Serial.read());
        if (received == '\n') {
            if (!discardingCommand && commandLength > 0) {
                applyCommand();
            }
            commandLength = 0;
            discardingCommand = false;
        } else if (received != '\r' && !discardingCommand) {
            if (commandLength + 1 < kCommandBufferSize) {
                commandBuffer[commandLength++] = received;
            } else {
                commandLength = 0;
                discardingCommand = true;
            }
        }
    }
}

}

void setup() {
    Serial.begin(kSerialBaudRate);
    motorReady = motorBus.begin() && motor.enable();
    lastCommandAt = millis();
}

void loop() {
    readCommands();
    if (motorRunning && millis() - lastCommandAt >= kCommandTimeoutMs) {
        motor.stop();
        motorRunning = false;
    }
}
