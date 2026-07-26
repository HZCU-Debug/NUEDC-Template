#include <cassert>
#include <string>
#include <vector>

#include "ui/menu.h"

class RecordingItem : public ui::Item {
public:
    explicit RecordingItem(const char* label)
        : ui::Item(label), setupCount(0), enterCount(0), loopCount(0), exitCount(0),
          lastEvent(ui::Event::None) {}

    void setup() override { ++setupCount; }

    void enter(Adafruit_GFX&) override { ++enterCount; }

    void loop(Adafruit_GFX&, ui::Event event) override {
        ++loopCount;
        lastEvent = event;
    }

    void exit() override { ++exitCount; }

    int setupCount;
    int enterCount;
    int loopCount;
    int exitCount;
    ui::Event lastEvent;
};

int main() {
    Adafruit_GFX display(240, 135);
    RecordingItem first("First");
    RecordingItem second("Second");
    RecordingItem third("Third");
    ui::Item* items[] = {&first, &second, &third};
    ui::Menu menu(display, items, 3);

    assert(menu.begin());
    assert(first.setupCount == 1);
    assert(second.setupCount == 1);
    assert(third.setupCount == 1);
    assert(display.printed ==
           std::vector<std::string>({"Demos", "First", "Second", "Third"}));
    assert(menu.begin());
    assert(first.setupCount == 1);
    assert(second.setupCount == 1);
    assert(third.setupCount == 1);

    menu.loop(ui::Event::Up);
    menu.loop(ui::Event::Select);
    assert(third.enterCount == 1);

    menu.loop();
    assert(third.loopCount == 1);
    assert(third.lastEvent == ui::Event::None);

    menu.loop(ui::Event::Back);
    assert(third.exitCount == 1);

    menu.loop(ui::Event::Select);
    assert(third.enterCount == 2);
    menu.loop(ui::Event::Back);

    menu.loop(ui::Event::Down);
    menu.loop(ui::Event::Select);
    assert(first.enterCount == 1);

    return 0;
}
