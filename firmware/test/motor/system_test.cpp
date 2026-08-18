#include <cassert>

#include <Arduino.h>

#include "config/hardware.h"
#include "motor/system.h"

HardwareSerial Serial2;

int main() {
    motor::Motor& x = motor::systemMotor(motor::Axis::X);
    motor::Motor& y = motor::systemMotor(motor::Axis::Y);
    motor::Motor& z = motor::systemMotor(motor::Axis::Z);
    motor::Motor& rotation = motor::systemMotor(motor::Axis::Rotation);

    assert(&x != &y && &x != &z && &x != &rotation);
    assert(&y != &z && &y != &rotation && &z != &rotation);
    assert(x.begin());
    assert(y.begin());

    Serial2.respondWith({0xC5, 0x01, 0xFA, 0x01, 0x00, 0xC1, 0x5C});
    assert(x.enable());
    assert(Serial2.rxPin == config::kPins.motorRx);

    Serial2.transmitted.clear();
    Serial2.respondWith({0xC5, 0x02, 0xFA, 0x01, 0x00, 0xC2, 0x5C});
    assert(y.enable());
    assert(Serial2.rxPin == config::kPins.motorYRx);

    return 0;
}
