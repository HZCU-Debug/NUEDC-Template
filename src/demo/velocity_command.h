#pragma once

#include <stdint.h>

namespace demo {

bool parseVelocityCommand(const char* line, int16_t& velocity);
int16_t velocityToRpm(int16_t velocity, uint16_t maximumRpm);

}
