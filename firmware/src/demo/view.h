#pragma once

#include <Adafruit_GFX.h>

namespace demo {
namespace view {

const uint16_t kBackgroundColor = 0x0000;
const uint16_t kTextColor = 0xFFFF;
const uint16_t kAccentColor = 0x07FF;

inline void beginPage(Adafruit_GFX& display, const char* title) {
    display.fillScreen(kBackgroundColor);
    display.setTextSize(2);
    display.setTextColor(kAccentColor);
    display.setCursor(6, 6);
    display.print(title);
}

inline void beginBody(Adafruit_GFX& display) {
    display.fillRect(0, 30, display.width(), display.height() - 30,
                     kBackgroundColor);
    display.setTextColor(kTextColor);
    display.setTextSize(2);
}

}
}
