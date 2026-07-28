/**
 * @file main.cpp
 * @brief Arduino 固件入口、屏幕菜单和按钮输入
 */
#include <Adafruit_ST7789.h>
#include <Arduino.h>
#include <SPI.h>

#include "program/demo/state_machine_demo.h"
#include "program/programs.h"
#include "runtime/program_runner.h"
#include "runtime/state_machine.h"
#include "ui/menu.h"

namespace {

const int8_t kDisplayClockPin = 18;
const int8_t kDisplayResetPin = 33;
const int8_t kDisplayDataPin = 23;
const int8_t kDisplayBacklightPin = 12;
const int8_t kDisplayDcPin = 27;
const int8_t kDisplayCsPin = 32;

const int8_t kUpButtonPin = 0;
const int8_t kDownButtonPin = 35;
const int8_t kSelectButtonPin = 34;
const int8_t kBackButtonPin = 39;
const uint32_t kDebounceMs = 25;

class Button {
public:
    Button(int8_t pin, ui::Event event)
        : pin_(pin),
          event_(event),
          lastReading_(false),
          stablePressed_(false),
          changedAt_(0) {}

    void begin() {
        pinMode(pin_, INPUT);
        stablePressed_ = digitalRead(pin_) == LOW;
        lastReading_ = stablePressed_;
        changedAt_ = millis();
    }

    ui::Event poll(uint32_t now) {
        const bool pressed = digitalRead(pin_) == LOW;
        if (pressed != lastReading_) {
            lastReading_ = pressed;
            changedAt_ = now;
        }
        if (pressed == stablePressed_ || now - changedAt_ < kDebounceMs) {
            return ui::Event::None;
        }

        stablePressed_ = pressed;
        return pressed ? event_ : ui::Event::None;
    }

private:
    int8_t pin_;
    ui::Event event_;
    bool lastReading_;
    bool stablePressed_;
    uint32_t changedAt_;
};

Adafruit_ST7789 display(kDisplayCsPin, kDisplayDcPin, kDisplayResetPin);
Button buttons[] = {
    Button(kUpButtonPin, ui::Event::Up),
    Button(kDownButtonPin, ui::Event::Down),
    Button(kSelectButtonPin, ui::Event::Select),
    Button(kBackButtonPin, ui::Event::Back),
};
ui::Menu* menu = nullptr;
runtime::SystemState systemState;
runtime::ProgramRunner programRunner(systemState);
runtime::StateMachine<program::DemoState> demoStateMachine(
    program::DemoState::EntryA);
program::StateMachineDemoProgram stateMachineDemo(Serial, demoStateMachine);

ui::Event readEvent(uint32_t now) {
    ui::Event result = ui::Event::None;
    for (size_t index = 0; index < sizeof(buttons) / sizeof(buttons[0]); ++index) {
        const ui::Event event = buttons[index].poll(now);
        if (result == ui::Event::None && event != ui::Event::None) {
            result = event;
        }
    }
    return result;
}

}

void setup() {
    for (size_t index = 0; index < sizeof(buttons) / sizeof(buttons[0]); ++index) {
        buttons[index].begin();
    }

    SPI.begin(kDisplayClockPin, -1, kDisplayDataPin, kDisplayCsPin);
    display.init(135, 240);
    display.setRotation(1);
    display.setTextWrap(false);
    pinMode(kDisplayBacklightPin, OUTPUT);
    digitalWrite(kDisplayBacklightPin, HIGH);

    static ui::Item controllerMotorItem(
        "Controller Motor", programRunner, program::controllerMotor());
    static ui::Item motorRampItem(
        "Motor Ramp", programRunner, program::motorRamp());
    static ui::Item motorPositionItem(
        "Motor Position", programRunner, program::motorPosition());
    static ui::Item commUnreliableItem(
        "Comm Unreliable", programRunner, program::commUnreliable());
    static ui::Item commReliableItem(
        "Comm Reliable", programRunner, program::commReliable());
    static ui::Item quaternionItem(
        "Quaternion", programRunner, program::quaternion());
    static ui::ConfiguredItem<program::StateMachineDemoProgram,
                              program::DemoConfiguration>
        stateMachineEntryA("State Entry A", programRunner, stateMachineDemo,
                           program::DemoConfiguration(program::DemoState::EntryA));
    static ui::ConfiguredItem<program::StateMachineDemoProgram,
                              program::DemoConfiguration>
        stateMachineEntryB("State Entry B", programRunner, stateMachineDemo,
                           program::DemoConfiguration(program::DemoState::EntryB));
    static ui::Item* items[] = {
        &controllerMotorItem,
        &motorRampItem,
        &motorPositionItem,
        &commUnreliableItem,
        &commReliableItem,
        &quaternionItem,
        &stateMachineEntryA,
        &stateMachineEntryB,
    };
    static ui::Menu appMenu(display, "NUEDC", items,
                            sizeof(items) / sizeof(items[0]));
    menu = &appMenu;
    menu->begin();
}

void loop() {
    systemState.now = millis();
    menu->loop(readEvent(systemState.now));
}
