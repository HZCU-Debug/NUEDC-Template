#include "platform/system_clock.h"

#include "platform/zf.h"

namespace platform {

SystemClock::SystemClock() : nowMs_(0) {}

void SystemClock::begin() {
    nowMs_ = 0;
}

uint32_t SystemClock::nowMs() const { return nowMs_; }

void SystemClock::delayMs(uint32_t durationMs) {
    system_delay_ms(durationMs);
    nowMs_ += durationMs;
}

}
