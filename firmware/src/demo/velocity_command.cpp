#include "velocity_command.h"

namespace demo {

bool decodeVelocityCommand(const uint8_t* payload, size_t size, int16_t& velocity) {
    if (payload == nullptr || size != 2) {
        return false;
    }

    const uint16_t encoded =
        static_cast<uint16_t>(static_cast<uint16_t>(payload[0]) << 8) | payload[1];
    const int32_t decoded =
        encoded <= INT16_MAX ? encoded : static_cast<int32_t>(encoded) - 0x10000L;
    if (decoded < -1000 || decoded > 1000) {
        return false;
    }

    velocity = static_cast<int16_t>(decoded);
    return true;
}

int16_t velocityToRpm(int16_t velocity, uint16_t maximumRpm) {
    return static_cast<int16_t>(static_cast<int32_t>(velocity) * maximumRpm / 1000);
}

}
