/**
 * @file comm_reliable.cpp
 * @brief 通过 USB 串口可靠发送计数消息并在确认后等待 500 ms 再发送下一条
 */
#include "demo.h"

#include <Arduino.h>

#include "comm/link.h"

namespace demo {
namespace commReliable {
namespace {

const uint32_t kSerialBaudRate = 115200;
const uint32_t kSendIntervalMs = 500;
const uint8_t kCounterMessage = 1;

comm::Link<4> link(Serial, comm::LinkConfig(kSerialBaudRate));
uint32_t counter = 0;
uint32_t lastDeliveredAt = 0;
bool waiting = false;
bool firstMessage = true;

void encodeCounter(uint8_t* payload) {
    payload[0] = static_cast<uint8_t>(counter >> 24);
    payload[1] = static_cast<uint8_t>(counter >> 16);
    payload[2] = static_cast<uint8_t>(counter >> 8);
    payload[3] = static_cast<uint8_t>(counter);
}

}

void setup() { link.begin(); }

void loop() {
    const comm::Event event = link.poll();
    if (event.type == comm::EventType::Delivered) {
        waiting = false;
        firstMessage = false;
        ++counter;
        lastDeliveredAt = millis();
    }

    const uint32_t now = millis();
    if (waiting || (!firstMessage && now - lastDeliveredAt < kSendIntervalMs)) {
        return;
    }

    uint8_t payload[4];
    encodeCounter(payload);
    waiting = link.send(kCounterMessage, payload, sizeof(payload),
                        comm::Delivery::Reliable) == comm::SendResult::Accepted;
}

}

}
