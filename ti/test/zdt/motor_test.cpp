#include <cassert>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <vector>

#include "support/fakes.h"
#include "zdt/motor.h"

static void expectBytes(const std::vector<uint8_t>& actual,
                        std::initializer_list<uint8_t> expected) {
    assert(actual == std::vector<uint8_t>(expected));
}

int main() {
    FakeStream stream;
    FakeClock clock;
    zdt::Bus bus(stream, clock, zdt::BusConfig(20));
    zdt::Motor motor(bus, zdt::MotorConfig(1, 3200));

    assert(bus.begin());
    assert(motor.enable());
    expectBytes(stream.output, {0x01, 0xF3, 0xAB, 0x01, 0x00, 0x6B});

    stream.output.clear();
    assert(motor.disable());
    expectBytes(stream.output, {0x01, 0xF3, 0xAB, 0x00, 0x00, 0x6B});

    stream.output.clear();
    assert(motor.run(-300, 10, zdt::Start::Synchronized));
    expectBytes(stream.output,
                {0x01, 0xF6, 0x01, 0x01, 0x2C, 0x0A, 0x01, 0x6B});

    stream.output.clear();
    assert(motor.moveRelative(
        -90.0f, zdt::MotionOptions(300, 10, zdt::Start::Synchronized)));
    expectBytes(stream.output,
                {0x01, 0xFD, 0x01, 0x01, 0x2C, 0x0A, 0x00, 0x00,
                 0x03, 0x20, 0x00, 0x01, 0x6B});

    stream.output.clear();
    assert(motor.moveAbsolute(180.0f, zdt::MotionOptions(200)));
    expectBytes(stream.output,
                {0x01, 0xFD, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00,
                 0x06, 0x40, 0x01, 0x00, 0x6B});

    stream.output.clear();
    assert(motor.stop());
    expectBytes(stream.output, {0x01, 0xFE, 0x98, 0x00, 0x6B});

    stream.output.clear();
    assert(motor.home(zdt::HomeMode::EndStop));
    expectBytes(stream.output, {0x01, 0x9A, 0x03, 0x00, 0x6B});

    stream.output.clear();
    assert(bus.triggerSynchronized());
    expectBytes(stream.output, {0x00, 0xFF, 0x66, 0x6B});

    stream.output.clear();
    stream.respondWith({0x01, 0x3A, 0x0F, 0x6B});
    stream.respondWith({0x01, 0x3B, 0x0C, 0x6B});
    const zdt::Result<zdt::MotorState> state = motor.readState();
    assert(state);
    assert(state.value.enabled);
    assert(state.value.reached);
    assert(state.value.stalled);
    assert(state.value.stallProtected);
    assert(state.value.homing);
    assert(state.value.homingFailed);

    stream.output.clear();
    stream.respondWith({0x01, 0x36, 0x00, 0x00, 0x00, 0x40, 0x00, 0x6B});
    const zdt::Result<float> position = motor.readPositionDegrees();
    assert(position);
    assert(std::fabs(position.value - 90.0f) < 0.001f);

    stream.output.clear();
    stream.respondWith({0x01, 0x35, 0x01, 0x01, 0x2C, 0x6B});
    const zdt::Result<float> speed = motor.readSpeedRpm();
    assert(speed);
    assert(speed.value == -300.0f);

    zdt::Motor invertedMotor(bus, zdt::MotorConfig(1, 3200, true));
    stream.output.clear();
    assert(invertedMotor.run(300));
    expectBytes(stream.output,
                {0x01, 0xF6, 0x01, 0x01, 0x2C, 0x00, 0x00, 0x6B});

    stream.output.clear();
    stream.respondWith({0x01, 0x3A, 0x03, 0x6B});
    stream.respondWith({0x01, 0x3B, 0x04, 0x6B});
    stream.respondWith({0x01, 0x36, 0x00, 0x00, 0x00, 0x40, 0x00, 0x6B});
    stream.respondWith({0x01, 0x35, 0x00, 0x01, 0x2C, 0x6B});
    const zdt::Result<zdt::MotorSnapshot> snapshot =
        invertedMotor.readSnapshot();
    assert(snapshot);
    assert(snapshot.value.state.enabled);
    assert(snapshot.value.state.reached);
    assert(snapshot.value.state.homing);
    assert(std::fabs(snapshot.value.positionDegrees + 90.0f) < 0.001f);
    assert(snapshot.value.speedRpm == -300.0f);

    assert(!motor.run(0));
    assert(!motor.run(5001));
    assert(!motor.moveAbsolute(0.0f, zdt::MotionOptions(100)));

    FakeStream timeoutStream;
    FakeClock timeoutClock;
    zdt::Bus timeoutBus(timeoutStream, timeoutClock, zdt::BusConfig(3));
    zdt::Motor timeoutMotor(timeoutBus, zdt::MotorConfig(1, 3200));
    assert(timeoutBus.begin());
    const zdt::Result<float> timeout = timeoutMotor.readPositionDegrees();
    assert(!timeout);
    assert(timeout.error == zdt::Error::Timeout);

    FakeStream errorStream;
    FakeClock errorClock;
    zdt::Bus errorBus(errorStream, errorClock);
    zdt::Motor errorMotor(errorBus, zdt::MotorConfig(1, 3200));
    assert(errorBus.begin());
    errorStream.respondWith({0x01, 0x00, 0xEE, 0x6B});
    const zdt::Result<float> invalid = errorMotor.readPositionDegrees();
    assert(!invalid);
    assert(invalid.error == zdt::Error::InvalidResponse);

    FakeStream failedStream;
    FakeClock failedClock;
    zdt::Bus failedBus(failedStream, failedClock);
    zdt::Motor failedMotor(failedBus, zdt::MotorConfig(1, 3200));
    assert(failedBus.begin());
    failedStream.failWrite = true;
    const zdt::Status failed = failedMotor.stop();
    assert(!failed);
    assert(failed.error == zdt::Error::WriteFailed);
}
