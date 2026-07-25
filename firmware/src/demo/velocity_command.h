#pragma once

#include <stddef.h>
#include <stdint.h>

namespace demo {

bool decodeVelocityCommand(const uint8_t* payload, size_t size, int16_t& velocity);
int16_t velocityToRpm(int16_t velocity, uint16_t maximumRpm);

}
