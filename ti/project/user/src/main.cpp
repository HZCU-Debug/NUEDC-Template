#include "platform/board.h"
#include "platform/ips114_display.h"
#include "platform/zf.h"
#include "program/programs.h"
#include "program/state_machine_demo.h"
#include "runtime/program_runner.h"
#include "ui/menu.h"

namespace {

ui::Event readEvent() {
    const key_index_enum keys[] = {KEY_1, KEY_2, KEY_3, KEY_4};
    const ui::Event events[] = {ui::Event::Up, ui::Event::Down,
                                ui::Event::Select, ui::Event::Back};
    for (size_t index = 0; index < KEY_NUMBER; ++index) {
        if (key_get_state(keys[index]) == KEY_SHORT_PRESS) {
            key_clear_state(keys[index]);
            return events[index];
        }
    }
    return ui::Event::None;
}

}

int main() {
    clock_init(SYSTEM_CLOCK_80M);
    system_delay_ms(300);

    platform::systemClock().begin();
    key_init(5);

    platform::Ips114Display display;
    display.begin();
    interrupt_global_enable(0);

    runtime::SystemState state;
    runtime::ProgramRunner runner(state);
    runtime::StateMachine<program::DemoState> stateMachine(
        program::DemoState::EntryA);
    program::StateMachineDemoProgram stateMachineDemo(stateMachine);

    ui::Item controllerMotor("Controller Motor", runner,
                             program::controllerMotor());
    ui::Item motorRamp("Motor Ramp", runner, program::motorRamp());
    ui::Item motorPosition("Motor Position", runner,
                           program::motorPosition());
    ui::Item commUnreliable("Comm Unreliable", runner,
                            program::commUnreliable());
    ui::Item commReliable("Comm Reliable", runner,
                          program::commReliable());
    ui::Item quaternion("Quaternion", runner, program::quaternion());
    ui::ConfiguredItem<program::StateMachineDemoProgram,
                       program::DemoConfiguration>
        stateEntryA("State Entry A", runner, stateMachineDemo,
                    program::DemoConfiguration(program::DemoState::EntryA));
    ui::ConfiguredItem<program::StateMachineDemoProgram,
                       program::DemoConfiguration>
        stateEntryB("State Entry B", runner, stateMachineDemo,
                    program::DemoConfiguration(program::DemoState::EntryB));
    ui::Item* items[] = {&controllerMotor, &motorRamp,      &motorPosition,
                         &commUnreliable,  &commReliable,   &quaternion,
                         &stateEntryA,     &stateEntryB};
    ui::Menu menu(display, "NUEDC TI", items,
                  sizeof(items) / sizeof(items[0]));
    menu.begin();

    while (true) {
        state.now = platform::systemClock().nowMs();
        key_scanner();
        menu.loop(readEvent());
        platform::systemClock().delayMs(5);
    }
}
