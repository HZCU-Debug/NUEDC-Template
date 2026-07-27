#include <cassert>

#include "program/state_machine_demo.h"
#include "support/fakes.h"

int main() {
    FakeDisplay display;
    runtime::SystemState state;
    runtime::StateMachine<program::DemoState> stateMachine(
        program::DemoState::EntryA);
    program::StateMachineDemoProgram demo(stateMachine);

    demo.configure(program::DemoConfiguration(program::DemoState::EntryB));
    demo.start(display, state);
    assert(stateMachine.state() == program::DemoState::EntryB);

    demo.update(display, state, ui::Event::None);
    assert(stateMachine.state() == program::DemoState::Shared);
    demo.update(display, state, ui::Event::None);
    assert(stateMachine.state() == program::DemoState::Loop);
    demo.update(display, state, ui::Event::None);
    assert(stateMachine.state() == program::DemoState::Shared);
}
