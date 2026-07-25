#include <cassert>

#include "demo/velocity_command.h"

int main() {
    int16_t velocity = 0;

    const uint8_t positive[] = {0x03, 0xE8};
    assert(demo::decodeVelocityCommand(positive, sizeof(positive), velocity));
    assert(velocity == 1000);
    assert(demo::velocityToRpm(velocity, 300) == 300);

    const uint8_t negative[] = {0xFE, 0x0C};
    assert(demo::decodeVelocityCommand(negative, sizeof(negative), velocity));
    assert(velocity == -500);
    assert(demo::velocityToRpm(velocity, 300) == -150);

    const uint8_t zero[] = {0x00, 0x00};
    assert(demo::decodeVelocityCommand(zero, sizeof(zero), velocity));
    assert(demo::velocityToRpm(velocity, 300) == 0);

    const uint8_t tooLarge[] = {0x03, 0xE9};
    assert(!demo::decodeVelocityCommand(tooLarge, sizeof(tooLarge), velocity));
    assert(!demo::decodeVelocityCommand(positive, 1, velocity));
    assert(!demo::decodeVelocityCommand(nullptr, 2, velocity));

    return 0;
}
