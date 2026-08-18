#include <algorithm>
#include <cassert>
#include <cstring>
#include <string>

#include "RecordingMotor.h"
#include "comm/link.h"
#include "config/parameters.h"
#include "program/motor_test.h"

HardwareSerial Serial;
HardwareSerial Serial2;

namespace {

void transfer(HardwareSerial& from, HardwareSerial& to) {
    to.receive(from.transmitted);
    from.transmitted.clear();
}

bool displayed(const Adafruit_GFX& display, const char* text) {
    return std::find(display.printed.begin(), display.printed.end(), text) !=
           display.printed.end();
}

}

int main() {
    HardwareSerial deviceSerial;
    HardwareSerial hostSerial;
    RecordingMotor x;
    RecordingMotor y;
    RecordingMotor z;
    RecordingMotor rotation;
    program::MotorTestProgram program(deviceSerial, x, y, z, rotation);
    comm::Link<7> hostLink(hostSerial);
    assert(hostLink.begin());

    runtime::SystemState state;
    Adafruit_GFX display(240, 135);
    program.start(display, state);
    assert(displayed(display, "Ready"));
    assert(x.beginCount == 1 && y.beginCount == 1 && z.beginCount == 1 &&
           rotation.beginCount == 1);

    const uint8_t speed[] = {'X', 0xFE, 0xD4};
    assert(hostLink.send(program::kMotorSpeedMessage, speed, sizeof(speed),
                         comm::Delivery::Reliable) ==
           comm::SendResult::Accepted);
    transfer(hostSerial, deviceSerial);
    program.update(display, state, ui::Event::None);
    assert(displayed(display, "Running"));
    assert(displayed(display, "X:-300 RPM"));
    assert(x.enabled && !y.enabled && !z.enabled && !rotation.enabled);
    assert(x.runCount == 1 && x.lastRpm == -300);
    assert(y.runCount == 0 && z.runCount == 0 && rotation.runCount == 0);
    transfer(deviceSerial, hostSerial);
    assert(hostLink.poll().type == comm::EventType::Delivered);

    const float targetDegrees = 90.0f;
    uint32_t encodedDegrees;
    std::memcpy(&encodedDegrees, &targetDegrees, sizeof(encodedDegrees));
    const uint8_t position[] = {
        'Y', static_cast<uint8_t>(encodedDegrees >> 24),
        static_cast<uint8_t>(encodedDegrees >> 16),
        static_cast<uint8_t>(encodedDegrees >> 8),
        static_cast<uint8_t>(encodedDegrees), 0x01, 0x2C};
    assert(hostLink.send(program::kMotorPositionMessage, position,
                         sizeof(position), comm::Delivery::Reliable) ==
           comm::SendResult::Accepted);
    transfer(hostSerial, deviceSerial);
    program.update(display, state, ui::Event::None);
    assert(displayed(display, "Y:90.00 deg"));
    assert(y.absoluteCount == 1 && y.lastDegrees == 90.0f);
    assert(y.lastOptions.rpm == 300);
    assert(y.lastOptions.acceleration == config::kMotorTestAcceleration);
    transfer(deviceSerial, hostSerial);
    assert(hostLink.poll().type == comm::EventType::Delivered);

    const uint8_t invalid[] = {'A', 0x01, 0x2C};
    assert(hostLink.send(program::kMotorSpeedMessage, invalid, sizeof(invalid),
                         comm::Delivery::Reliable) ==
           comm::SendResult::Accepted);
    transfer(hostSerial, deviceSerial);
    program.update(display, state, ui::Event::None);
    assert(displayed(display, "Bad command"));
    assert(x.runCount == 1 && y.runCount == 0);
    transfer(deviceSerial, hostSerial);
    assert(hostLink.poll().type == comm::EventType::Delivered);

    const uint8_t stop[] = {'X', 0x00, 0x00};
    assert(hostLink.send(program::kMotorSpeedMessage, stop, sizeof(stop),
                         comm::Delivery::Reliable) ==
           comm::SendResult::Accepted);
    transfer(hostSerial, deviceSerial);
    program.update(display, state, ui::Event::None);
    assert(x.stopCount == 1);

    program.requestExit();
    assert(program.readyToExit());
    assert(x.stopCount == 2 && y.stopCount == 1 && z.stopCount == 1 &&
           rotation.stopCount == 1);
    program.stop(state);
    assert(x.stopCount == 2 && y.stopCount == 1 && z.stopCount == 1 &&
           rotation.stopCount == 1);

    return 0;
}
