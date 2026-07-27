#include "program/programs.h"

#include <limits.h>

#include "comm/link.h"
#include "platform/board.h"
#include "ui/view.h"
#include "zdt/motor.h"

namespace program {
namespace {

const uint8_t kMotorAddress = 1;
const uint32_t kPulsesPerRevolution = 3200;

class ControllerMotorProgram final : public runtime::Program {
public:
    ControllerMotorProgram()
        : motorBus_(platform::motorUart(), platform::systemClock()),
          motor_(motorBus_,
                 zdt::MotorConfig(kMotorAddress, kPulsesPerRevolution)),
          controllerLink_(platform::communicationUart(),
                          platform::systemClock()),
          linkReady_(false),
          motorReady_(false),
          motorRunning_(false),
          targetRpm_(0),
          lastCommandAt_(0) {}

    void start(ui::Display& display, runtime::SystemState& state) override {
        linkReady_ = controllerLink_.begin();
        zdt::Status status(zdt::Error::NotStarted);
        if (linkReady_) {
            status = motorBus_.begin();
        }
        if (status) {
            platform::systemClock().delayMs(2000);
            status = motor_.enable();
        }
        if (status) {
            const zdt::Result<float> position = motor_.readPositionDegrees();
            status = zdt::Status(position.error);
        }
        motorReady_ = static_cast<bool>(status);
        motorRunning_ = false;
        targetRpm_ = 0;
        lastCommandAt_ = state.now;
        ui::view::beginPage(display, "Controller Motor");
        render(display);
    }

    void update(ui::Display& display, runtime::SystemState& state,
                ui::Event) override {
        bool changed = readCommands(state.now);
        if (motorRunning_ && state.now - lastCommandAt_ >= 500) {
            motor_.stop();
            motorRunning_ = false;
            targetRpm_ = 0;
            changed = true;
        }
        if (changed) {
            render(display);
        }
    }

    void stop(runtime::SystemState&) override {
        controllerLink_.cancel();
        if (motorReady_) {
            motor_.stop();
        }
        motorRunning_ = false;
        targetRpm_ = 0;
    }

private:
    bool decodeVelocity(const uint8_t* payload, size_t size,
                        int16_t& velocity) const {
        if (payload == nullptr || size != 2) {
            return false;
        }
        const uint16_t encoded =
            static_cast<uint16_t>(static_cast<uint16_t>(payload[0]) << 8) |
            payload[1];
        const int32_t decoded =
            encoded <= INT16_MAX ? encoded
                                 : static_cast<int32_t>(encoded) - 0x10000L;
        if (decoded < -1000 || decoded > 1000) {
            return false;
        }
        velocity = static_cast<int16_t>(decoded);
        return true;
    }

    bool applyCommand(const comm::MessageView& message, uint32_t now) {
        int16_t velocity = 0;
        if (!motorReady_ || message.type != 1 ||
            !decodeVelocity(message.payload, message.size, velocity)) {
            return false;
        }

        const int16_t rpm = static_cast<int16_t>(
            static_cast<int32_t>(velocity) * 300 / 1000);
        const zdt::Status status =
            rpm == 0 ? motor_.stop() : motor_.run(rpm);
        if (!status) {
            motorReady_ = false;
            motorRunning_ = false;
            targetRpm_ = 0;
            return true;
        }
        motorRunning_ = rpm != 0;
        targetRpm_ = rpm;
        lastCommandAt_ = now;
        return true;
    }

    bool readCommands(uint32_t now) {
        bool changed = false;
        for (;;) {
            const comm::Event event = controllerLink_.poll();
            if (event.type == comm::EventType::None) {
                return changed;
            }
            if (event.type == comm::EventType::Message) {
                changed = applyCommand(event.message, now) || changed;
            }
        }
    }

    void render(ui::Display& display) const {
        ui::view::beginBody(display);
        display.text(4, 36,
                     !linkReady_ ? "Comm error"
                                 : motorReady_ ? "Motor ready" : "Motor error",
                     ui::view::kTextColor, ui::view::kBackgroundColor);
        display.text(4, 58, "RPM:", ui::view::kTextColor,
                     ui::view::kBackgroundColor);
        display.integer(52, 58, targetRpm_, ui::view::kTextColor,
                        ui::view::kBackgroundColor);
    }

    zdt::Bus motorBus_;
    zdt::Motor motor_;
    comm::Link<2> controllerLink_;
    bool linkReady_;
    bool motorReady_;
    bool motorRunning_;
    int16_t targetRpm_;
    uint32_t lastCommandAt_;
};

class MotorRampProgram final : public runtime::Program {
public:
    MotorRampProgram()
        : motorBus_(platform::motorUart(), platform::systemClock()),
          motor_(motorBus_,
                 zdt::MotorConfig(kMotorAddress, kPulsesPerRevolution)),
          motorReady_(false),
          targetRpm_(0),
          direction_(1),
          lastStepAt_(0) {}

    void start(ui::Display& display, runtime::SystemState& state) override {
        zdt::Status status = motorBus_.begin();
        if (status) {
            platform::systemClock().delayMs(2000);
            status = motor_.enable();
        }
        motorReady_ = static_cast<bool>(status);
        targetRpm_ = 0;
        direction_ = 1;
        lastStepAt_ = state.now;
        ui::view::beginPage(display, "Motor Ramp");
        render(display);
    }

    void update(ui::Display& display, runtime::SystemState& state,
                ui::Event) override {
        if (!motorReady_ || state.now - lastStepAt_ < 100) {
            return;
        }

        targetRpm_ = static_cast<int16_t>(targetRpm_ + direction_ * 10);
        if (targetRpm_ == 300) {
            direction_ = -1;
        } else if (targetRpm_ == -300) {
            direction_ = 1;
        }

        const zdt::Status status =
            targetRpm_ == 0 ? motor_.stop() : motor_.run(targetRpm_);
        motorReady_ = static_cast<bool>(status);
        if (!motorReady_) {
            targetRpm_ = 0;
        }
        lastStepAt_ = state.now;
        render(display);
    }

    void stop(runtime::SystemState&) override {
        if (motorReady_) {
            motor_.stop();
        }
        targetRpm_ = 0;
    }

private:
    void render(ui::Display& display) const {
        ui::view::beginBody(display);
        display.text(4, 36, motorReady_ ? "Motor ready" : "Motor error",
                     ui::view::kTextColor, ui::view::kBackgroundColor);
        display.text(4, 58, "RPM:", ui::view::kTextColor,
                     ui::view::kBackgroundColor);
        display.integer(52, 58, targetRpm_, ui::view::kTextColor,
                        ui::view::kBackgroundColor);
    }

    zdt::Bus motorBus_;
    zdt::Motor motor_;
    bool motorReady_;
    int16_t targetRpm_;
    int8_t direction_;
    uint32_t lastStepAt_;
};

class MotorPositionProgram final : public runtime::Program {
public:
    MotorPositionProgram()
        : motorBus_(platform::motorUart(), platform::systemClock()),
          motor_(motorBus_,
                 zdt::MotorConfig(kMotorAddress, kPulsesPerRevolution)),
          motorReady_(false),
          lastReadAt_(0),
          positionDegrees_(0.0f),
          error_(zdt::Error::None) {}

    void start(ui::Display& display, runtime::SystemState& state) override {
        positionDegrees_ = 0.0f;
        zdt::Status status = motorBus_.begin();
        if (status) {
            platform::systemClock().delayMs(2000);
            status = motor_.disable();
        }
        motorReady_ = static_cast<bool>(status);
        error_ = status.error;
        lastReadAt_ = state.now;
        ui::view::beginPage(display, "Motor Position");
        render(display);
    }

    void update(ui::Display& display, runtime::SystemState& state,
                ui::Event) override {
        if (!motorReady_ || state.now - lastReadAt_ < 500) {
            return;
        }

        const zdt::Result<float> position = motor_.readPositionDegrees();
        error_ = position.error;
        if (position) {
            positionDegrees_ = position.value;
        }
        lastReadAt_ = state.now;
        render(display);
    }

private:
    void render(ui::Display& display) const {
        ui::view::beginBody(display);
        if (error_ == zdt::Error::None) {
            display.text(4, 36, "Angle:", ui::view::kTextColor,
                         ui::view::kBackgroundColor);
            display.decimal(60, 36, positionDegrees_, ui::view::kTextColor,
                            ui::view::kBackgroundColor);
        } else {
            display.text(4, 36, "Error:", ui::view::kTextColor,
                         ui::view::kBackgroundColor);
            display.integer(60, 36, static_cast<int32_t>(error_),
                            ui::view::kTextColor,
                            ui::view::kBackgroundColor);
        }
    }

    zdt::Bus motorBus_;
    zdt::Motor motor_;
    bool motorReady_;
    uint32_t lastReadAt_;
    float positionDegrees_;
    zdt::Error error_;
};

class CommUnreliableProgram final : public runtime::Program {
public:
    CommUnreliableProgram()
        : link_(platform::communicationUart(), platform::systemClock()),
          ready_(false),
          counter_(0),
          lastSentAt_(0) {}

    void start(ui::Display& display, runtime::SystemState& state) override {
        counter_ = 0;
        ready_ = link_.begin();
        lastSentAt_ = state.now;
        ui::view::beginPage(display, "Comm Unreliable");
        render(display);
    }

    void update(ui::Display& display, runtime::SystemState& state,
                ui::Event) override {
        link_.poll();
        if (!ready_ || state.now - lastSentAt_ < 500) {
            return;
        }

        uint8_t payload[4];
        encodeCounter(payload);
        if (link_.send(1, payload, sizeof(payload),
                       comm::Delivery::Unreliable) ==
            comm::SendResult::Accepted) {
            ++counter_;
        }
        lastSentAt_ = state.now;
        render(display);
    }

private:
    void encodeCounter(uint8_t* payload) const {
        payload[0] = static_cast<uint8_t>(counter_ >> 24);
        payload[1] = static_cast<uint8_t>(counter_ >> 16);
        payload[2] = static_cast<uint8_t>(counter_ >> 8);
        payload[3] = static_cast<uint8_t>(counter_);
    }

    void render(ui::Display& display) const {
        ui::view::beginBody(display);
        display.text(4, 36, ready_ ? "Sent:" : "Comm error",
                     ui::view::kTextColor, ui::view::kBackgroundColor);
        if (ready_) {
            display.integer(60, 36, static_cast<int32_t>(counter_),
                            ui::view::kTextColor,
                            ui::view::kBackgroundColor);
        }
    }

    comm::Link<4> link_;
    bool ready_;
    uint32_t counter_;
    uint32_t lastSentAt_;
};

class CommReliableProgram final : public runtime::Program {
public:
    CommReliableProgram()
        : link_(platform::communicationUart(), platform::systemClock()),
          ready_(false),
          counter_(0),
          lastDeliveredAt_(0),
          waiting_(false),
          firstMessage_(true) {}

    void start(ui::Display& display, runtime::SystemState& state) override {
        counter_ = 0;
        waiting_ = false;
        firstMessage_ = true;
        ready_ = link_.begin();
        lastDeliveredAt_ = state.now;
        ui::view::beginPage(display, "Comm Reliable");
        render(display);
    }

    void update(ui::Display& display, runtime::SystemState& state,
                ui::Event) override {
        const comm::Event event = link_.poll();
        if (event.type == comm::EventType::Delivered) {
            waiting_ = false;
            firstMessage_ = false;
            ++counter_;
            lastDeliveredAt_ = state.now;
            render(display);
        }

        if (!ready_ || waiting_ ||
            (!firstMessage_ && state.now - lastDeliveredAt_ < 500)) {
            return;
        }

        uint8_t payload[4];
        encodeCounter(payload);
        waiting_ = link_.send(1, payload, sizeof(payload),
                              comm::Delivery::Reliable) ==
                   comm::SendResult::Accepted;
        render(display);
    }

    void stop(runtime::SystemState&) override {
        link_.cancel();
        waiting_ = false;
    }

private:
    void encodeCounter(uint8_t* payload) const {
        payload[0] = static_cast<uint8_t>(counter_ >> 24);
        payload[1] = static_cast<uint8_t>(counter_ >> 16);
        payload[2] = static_cast<uint8_t>(counter_ >> 8);
        payload[3] = static_cast<uint8_t>(counter_);
    }

    void render(ui::Display& display) const {
        ui::view::beginBody(display);
        display.text(4, 36, ready_ ? "Count:" : "Comm error",
                     ui::view::kTextColor, ui::view::kBackgroundColor);
        if (ready_) {
            display.integer(60, 36, static_cast<int32_t>(counter_),
                            ui::view::kTextColor,
                            ui::view::kBackgroundColor);
            display.text(4, 58,
                         waiting_ ? "Waiting"
                                  : firstMessage_ ? "Ready" : "Delivered",
                         ui::view::kTextColor, ui::view::kBackgroundColor);
        }
    }

    comm::Link<4> link_;
    bool ready_;
    uint32_t counter_;
    uint32_t lastDeliveredAt_;
    bool waiting_;
    bool firstMessage_;
};

}

runtime::Program& controllerMotor() {
    static ControllerMotorProgram program;
    return program;
}

runtime::Program& motorRamp() {
    static MotorRampProgram program;
    return program;
}

runtime::Program& motorPosition() {
    static MotorPositionProgram program;
    return program;
}

runtime::Program& commUnreliable() {
    static CommUnreliableProgram program;
    return program;
}

runtime::Program& commReliable() {
    static CommReliableProgram program;
    return program;
}

}
