/**
 * @file comm_unreliable.cpp
 * @brief 每 500 ms 通过 USB 串口发送一次不等待确认或重传的非可靠计数消息
 */
#include "items.h"

#include <Arduino.h>

#include "comm/link.h"
#include "ui/view.h"

namespace item {
namespace {

const uint32_t kSerialBaudRate = 115200;
const uint32_t kSendIntervalMs = 500;
const uint8_t kCounterMessage = 1;

class CommUnreliableItem final : public ui::Item {
public:
    CommUnreliableItem()
        : ui::Item("Comm Unreliable"),
          link_(Serial, comm::LinkConfig(kSerialBaudRate)),
          counter_(0),
          lastSentAt_(0) {}

    void setup() override {
        counter_ = 0;
        lastSentAt_ = 0;
    }

    void enter(Adafruit_GFX& display) override {
        link_.begin();
        lastSentAt_ = millis();
        ui::view::beginPage(display, label());
        render(display);
    }

    void loop(Adafruit_GFX& display, ui::Event) override {
        link_.poll();
        const uint32_t now = millis();
        if (now - lastSentAt_ < kSendIntervalMs) {
            return;
        }

        uint8_t payload[4];
        encodeCounter(payload);
        if (link_.send(kCounterMessage, payload, sizeof(payload),
                       comm::Delivery::Unreliable) == comm::SendResult::Accepted) {
            ++counter_;
            render(display);
        }
        lastSentAt_ = now;
    }

private:
    void encodeCounter(uint8_t* payload) const {
        payload[0] = static_cast<uint8_t>(counter_ >> 24);
        payload[1] = static_cast<uint8_t>(counter_ >> 16);
        payload[2] = static_cast<uint8_t>(counter_ >> 8);
        payload[3] = static_cast<uint8_t>(counter_);
    }

    void render(Adafruit_GFX& display) const {
        ui::view::beginBody(display);
        display.setCursor(6, 42);
        display.print("Sent: ");
        display.print(counter_);
    }

    comm::Link<4> link_;
    uint32_t counter_;
    uint32_t lastSentAt_;
};

CommUnreliableItem item;

}

ui::Item& commUnreliable() { return item; }

}
