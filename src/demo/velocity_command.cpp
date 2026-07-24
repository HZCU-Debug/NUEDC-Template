#include "velocity_command.h"

#include <stdlib.h>

namespace demo {

bool parseVelocityCommand(const char* line, int16_t& velocity) {
    if (line[0] != 'V' || line[1] != ' ') {
        return false;
    }

    char* end = nullptr;
    const long parsed = strtol(line + 2, &end, 10);
    if (end == line + 2 || *end != '\0' || parsed < -1000 || parsed > 1000) {
        return false;
    }

    velocity = static_cast<int16_t>(parsed);
    return true;
}

int16_t velocityToRpm(int16_t velocity, uint16_t maximumRpm) {
    return static_cast<int16_t>(static_cast<int32_t>(velocity) * maximumRpm / 1000);
}

}
