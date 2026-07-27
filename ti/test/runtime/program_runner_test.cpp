#include <cassert>

#include "runtime/program_runner.h"
#include "support/fakes.h"

class RecordingProgram final : public runtime::Program {
public:
    RecordingProgram() : starts(0), updates(0), stops(0) {}

    void start(ui::Display&, runtime::SystemState&) override { ++starts; }

    void update(ui::Display&, runtime::SystemState&, ui::Event) override {
        ++updates;
    }

    void stop(runtime::SystemState&) override { ++stops; }

    int starts;
    int updates;
    int stops;
};

int main() {
    FakeDisplay display;
    runtime::SystemState state;
    runtime::ProgramRunner runner(state);
    RecordingProgram first;
    RecordingProgram second;

    runner.start(first, display);
    runner.update(display, ui::Event::Select);
    assert(first.starts == 1);
    assert(first.updates == 1);

    runner.start(second, display);
    assert(first.stops == 1);
    assert(second.starts == 1);

    runner.stop();
    runner.update(display, ui::Event::None);
    assert(second.stops == 1);
    assert(second.updates == 0);
}
