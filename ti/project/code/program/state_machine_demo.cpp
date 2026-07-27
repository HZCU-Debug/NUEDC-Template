#include "program/state_machine_demo.h"

#include "ui/view.h"

namespace program {
namespace {

const char* stateName(DemoState state) {
    switch (state) {
        case DemoState::EntryA:
            return "entry-a";
        case DemoState::EntryB:
            return "entry-b";
        case DemoState::Shared:
            return "shared";
        case DemoState::Loop:
            return "loop";
    }
    return "unknown";
}

}

StateMachineDemoProgram::StateMachineDemoProgram(
    runtime::StateMachine<DemoState>& stateMachine)
    : stateMachine_(stateMachine), configuration_(DemoState::EntryA) {}

void StateMachineDemoProgram::configure(
    const DemoConfiguration& configuration) {
    configuration_ = configuration;
}

void StateMachineDemoProgram::start(ui::Display& display,
                                    runtime::SystemState&) {
    stateMachine_.reset(configuration_.entry);
    ui::view::beginPage(display, "State Machine");
    render(display);
}

void StateMachineDemoProgram::update(ui::Display& display,
                                     runtime::SystemState&, ui::Event) {
    switch (stateMachine_.state()) {
        case DemoState::EntryA:
        case DemoState::EntryB:
            stateMachine_.transitionTo(DemoState::Shared);
            break;
        case DemoState::Shared:
            stateMachine_.transitionTo(DemoState::Loop);
            break;
        case DemoState::Loop:
            stateMachine_.transitionTo(DemoState::Shared);
            break;
    }
    render(display);
}

void StateMachineDemoProgram::render(ui::Display& display) const {
    ui::view::beginBody(display);
    display.text(4, 36, "State:", ui::view::kTextColor,
                 ui::view::kBackgroundColor);
    display.text(60, 36, stateName(stateMachine_.state()),
                 ui::view::kTextColor, ui::view::kBackgroundColor);
}

}
