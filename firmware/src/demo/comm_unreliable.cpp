/**
 * @file comm_unreliable.cpp
 * @brief 每 500 ms 通过 USB 串口发送一次不等待确认或重传的非可靠计数消息
 */
#include "demo.h"

#include <Arduino.h>

#include "comm/link.h"

namespace demo {
namespace commUnreliable {
namespace {

const uint32_t kSerialBaudRate = 115200;
const uint32_t kSendIntervalMs = 500;
const uint8_t kCounterMessage = 1;

comm::Link<4> link(Serial, comm::LinkConfig(kSerialBaudRate));
uint32_t counter = 0;
uint32_t lastSentAt = 0;

void encodeCounter(uint8_t* payload) {
    payload[0] = static_cast<uint8_t>(counter >> 24);
    payload[1] = static_cast<uint8_t>(counter >> 16);
    payload[2] = static_cast<uint8_t>(counter >> 8);
    payload[3] = static_cast<uint8_t>(counter);
}

}

void setup() {
    link.begin();
    lastSentAt = millis();
}

void loop() {
    link.poll();
    const uint32_t now = millis();
    if (now - lastSentAt < kSendIntervalMs) {
        return;
    }

    uint8_t payload[4];
    encodeCounter(payload);
    if (link.send(kCounterMessage, payload, sizeof(payload),
                  comm::Delivery::Unreliable) == comm::SendResult::Accepted) {
        ++counter;
    }
    lastSentAt = now;
}

}

}
