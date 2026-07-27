#include <cassert>

#include "platform/ips114_display.h"
#include "platform/zf.h"

int main() {
    platform::Ips114Display display;

    ips114_fake::reset();
    assert(display.begin());
    assert(ips114_fake::initCalls == 1);
    assert(ips114_fake::setDirCalls == 0);

    ips114_fake::reset();
    display.integer(0, 0, -2147483647, 0xFFFF, 0x0000);
    assert(ips114_fake::integerDigits == 10);

    ips114_fake::reset();
    display.decimal(0, 0, -12345678.25F, 0xFFFF, 0x0000);
    assert(ips114_fake::decimalIntegerDigits == 8);
    assert(ips114_fake::decimalFractionDigits == 2);
}
