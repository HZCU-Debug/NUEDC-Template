#include <cassert>

#include "demo/velocity_command.h"

int main() {
    int16_t velocity = 0;

    assert(demo::parseVelocityCommand("V 1000", velocity));
    assert(velocity == 1000);
    assert(demo::velocityToRpm(velocity, 300) == 300);

    assert(demo::parseVelocityCommand("V -500", velocity));
    assert(demo::velocityToRpm(velocity, 300) == -150);

    assert(demo::parseVelocityCommand("V 0", velocity));
    assert(demo::velocityToRpm(velocity, 300) == 0);

    assert(!demo::parseVelocityCommand("V 1001", velocity));
    assert(!demo::parseVelocityCommand("speed 100", velocity));
    assert(!demo::parseVelocityCommand("V 10 extra", velocity));

    return 0;
}
