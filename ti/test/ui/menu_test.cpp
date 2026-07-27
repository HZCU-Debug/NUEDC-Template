#include <cassert>

#include "runtime/program_runner.h"
#include "support/fakes.h"
#include "ui/menu.h"

class RecordingProgram final : public runtime::Program {
public:
    RecordingProgram()
        : starts(0), updates(0), stops(0), event(ui::Event::None) {}

    void start(ui::Display&, runtime::SystemState&) override { ++starts; }

    void update(ui::Display&, runtime::SystemState&, ui::Event next) override {
        ++updates;
        event = next;
    }

    void stop(runtime::SystemState&) override { ++stops; }

    int starts;
    int updates;
    int stops;
    ui::Event event;
};

int main() {
    FakeDisplay display;
    runtime::SystemState state;
    runtime::ProgramRunner runner(state);
    RecordingProgram firstProgram;
    RecordingProgram secondProgram;
    RecordingProgram thirdProgram;
    ui::Item first("First", runner, firstProgram);
    ui::Item second("Second", runner, secondProgram);
    ui::Item third("Third", runner, thirdProgram);
    ui::Item* items[] = {&first, &second, &third};
    ui::Menu menu(display, "Test Menu", items, 3);

    assert(menu.begin());
    assert(menu.begin());

    menu.loop(ui::Event::Up);
    menu.loop(ui::Event::Select);
    assert(thirdProgram.starts == 1);

    menu.loop(ui::Event::Down);
    assert(thirdProgram.updates == 1);
    assert(thirdProgram.event == ui::Event::Down);

    menu.loop(ui::Event::Back);
    assert(thirdProgram.stops == 1);

    menu.loop(ui::Event::Down);
    menu.loop(ui::Event::Select);
    assert(firstProgram.starts == 1);
}
