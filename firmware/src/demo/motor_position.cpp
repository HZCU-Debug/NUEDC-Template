/**
 * @file motor_position.cpp
 * @brief 失能电机并每 500 ms 显示和输出一次转子角度
 */
#include "demo.h"

#include <Arduino.h>

#include "view.h"
#include "zdt/motor.h"

namespace demo {
namespace {

const uint32_t kSerialBaudRate = 115200;
const int8_t kMotorRxPin = 25;
const int8_t kMotorTxPin = 26;
const uint8_t kMotorAddress = 1;
const uint32_t kPulsesPerRevolution = 3200;
const uint32_t kReadIntervalMs = 500;

class MotorPositionItem final : public ui::Item {
public:
    MotorPositionItem()
        : ui::Item("Motor Position"),
          motorBus_(Serial2,
                    zdt::BusConfig(kSerialBaudRate, kMotorRxPin, kMotorTxPin)),
          motor_(motorBus_,
                 zdt::MotorConfig(kMotorAddress, kPulsesPerRevolution)),
          motorReady_(false),
          lastReadAt_(0),
          positionDegrees_(0.0f),
          error_(zdt::Error::None) {}

    void setup() override {
        motorReady_ = false;
        lastReadAt_ = 0;
        positionDegrees_ = 0.0f;
        error_ = zdt::Error::None;
    }

    void enter(Adafruit_GFX& display) override {
        Serial.begin(kSerialBaudRate);
        zdt::Status status = motorBus_.begin();
        if (status) {
            delay(2000);
            status = motor_.disable();
        }
        motorReady_ = static_cast<bool>(status);
        error_ = status.error;
        if (!motorReady_) {
            Serial.printf("disable_error=%u\n", static_cast<unsigned>(error_));
        }
        lastReadAt_ = millis();
        view::beginPage(display, label());
        render(display);
    }

    void loop(Adafruit_GFX& display, ui::Event) override {
        const uint32_t now = millis();
        if (!motorReady_ || now - lastReadAt_ < kReadIntervalMs) {
            return;
        }

        const zdt::Result<float> position = motor_.readPositionDegrees();
        error_ = position.error;
        if (position) {
            positionDegrees_ = position.value;
            Serial.printf("%.2f\n", position.value);
        } else {
            Serial.printf("position_error=%u\n",
                          static_cast<unsigned>(position.error));
        }
        lastReadAt_ = now;
        render(display);
    }

private:
    void render(Adafruit_GFX& display) const {
        view::beginBody(display);
        display.setCursor(6, 42);
        if (error_ == zdt::Error::None) {
            display.print("Angle: ");
            display.print(positionDegrees_, 2);
        } else {
            display.print("Error: ");
            display.print(static_cast<unsigned>(error_));
        }
    }

    zdt::Bus motorBus_;
    zdt::Motor motor_;
    bool motorReady_;
    uint32_t lastReadAt_;
    float positionDegrees_;
    zdt::Error error_;
};

MotorPositionItem item;

}

ui::Item& motorPosition() { return item; }

}
