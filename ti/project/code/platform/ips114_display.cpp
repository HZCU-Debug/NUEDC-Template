#include "platform/ips114_display.h"

#include "platform/zf.h"

namespace platform {

bool Ips114Display::begin() {
    ips114_init();
    ips114_set_font(IPS114_8X16_FONT);
    clear(RGB565_BLACK);
    return true;
}

void Ips114Display::clear(uint16_t color) { ips114_full(color); }

void Ips114Display::text(uint16_t x, uint16_t y, const char* value,
                         uint16_t foreground, uint16_t background) {
    if (value == nullptr) {
        return;
    }
    ips114_set_color(foreground, background);
    ips114_show_string(x, y, value);
}

void Ips114Display::integer(uint16_t x, uint16_t y, int32_t value,
                            uint16_t foreground, uint16_t background) {
    ips114_set_color(foreground, background);
    ips114_show_int(x, y, value, 10);
}

void Ips114Display::decimal(uint16_t x, uint16_t y, float value,
                            uint16_t foreground, uint16_t background) {
    ips114_set_color(foreground, background);
    ips114_show_float(x, y, value, 8, 2);
}

uint16_t Ips114Display::width() const { return 240; }

uint16_t Ips114Display::height() const { return 135; }

}
