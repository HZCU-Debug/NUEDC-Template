#include "ui/menu.h"

#include "runtime/program_runner.h"

namespace ui {
namespace {

const uint16_t kBackgroundColor = 0x0000;
const uint16_t kTextColor = 0xFFFF;
const uint16_t kSelectedColor = 0x07FF;
const uint16_t kTitleColor = 0xFFE0;
const uint16_t kTitleHeight = 24;
const uint16_t kRowHeight = 18;

}

Item::Item(const char* label, runtime::ProgramRunner& runner,
           runtime::Program& program)
    : runner_(runner), program_(program), label_(label) {}

const char* Item::label() const { return label_; }

void Item::enter(Display& display) { runner_.start(program_, display); }

void Item::loop(Display& display, Event event) {
    runner_.update(display, event);
}

void Item::exit() { runner_.stop(); }

Menu::Menu(Display& display, const char* title, Item* const* items,
           size_t itemCount)
    : display_(display),
      title_(title),
      items_(items),
      itemCount_(itemCount),
      selectedIndex_(0),
      firstVisibleIndex_(0),
      activeItem_(nullptr),
      begun_(false) {}

bool Menu::begin() {
    if (begun_) {
        return true;
    }
    if (title_ == nullptr || items_ == nullptr || itemCount_ == 0) {
        return false;
    }
    for (size_t index = 0; index < itemCount_; ++index) {
        if (items_[index] == nullptr || items_[index]->label() == nullptr) {
            return false;
        }
    }
    begun_ = true;
    renderMenu();
    return true;
}

void Menu::loop(Event event) {
    if (!begun_) {
        return;
    }
    if (activeItem_ != nullptr) {
        if (event == Event::Back) {
            activeItem_->exit();
            activeItem_ = nullptr;
            renderMenu();
        } else {
            activeItem_->loop(display_, event);
        }
        return;
    }

    switch (event) {
        case Event::Up:
            moveSelection(-1);
            renderMenu();
            break;
        case Event::Down:
            moveSelection(1);
            renderMenu();
            break;
        case Event::Select:
            activeItem_ = items_[selectedIndex_];
            display_.clear(kBackgroundColor);
            activeItem_->enter(display_);
            break;
        case Event::None:
        case Event::Back:
            break;
    }
}

void Menu::moveSelection(int8_t offset) {
    if (offset < 0) {
        selectedIndex_ =
            selectedIndex_ == 0 ? itemCount_ - 1 : selectedIndex_ - 1;
    } else {
        selectedIndex_ =
            selectedIndex_ + 1 == itemCount_ ? 0 : selectedIndex_ + 1;
    }

    const size_t visibleRows =
        static_cast<size_t>((display_.height() - kTitleHeight) / kRowHeight);
    if (selectedIndex_ < firstVisibleIndex_) {
        firstVisibleIndex_ = selectedIndex_;
    } else if (selectedIndex_ >= firstVisibleIndex_ + visibleRows) {
        firstVisibleIndex_ = selectedIndex_ - visibleRows + 1;
    }
}

void Menu::renderMenu() {
    display_.clear(kBackgroundColor);
    display_.text(4, 3, title_, kTitleColor, kBackgroundColor);

    const size_t visibleRows =
        static_cast<size_t>((display_.height() - kTitleHeight) / kRowHeight);
    const size_t end = firstVisibleIndex_ + visibleRows < itemCount_
                           ? firstVisibleIndex_ + visibleRows
                           : itemCount_;
    for (size_t index = firstVisibleIndex_; index < end; ++index) {
        const uint16_t y = static_cast<uint16_t>(
            kTitleHeight + (index - firstVisibleIndex_) * kRowHeight);
        const bool selected = index == selectedIndex_;
        display_.text(4, y, selected ? ">" : " ",
                      selected ? kSelectedColor : kTextColor,
                      kBackgroundColor);
        display_.text(16, y, items_[index]->label(),
                      selected ? kSelectedColor : kTextColor,
                      kBackgroundColor);
    }
}

}
