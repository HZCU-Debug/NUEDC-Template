#pragma once

#include <stddef.h>
#include <stdint.h>

#include <vector>
#include <string>

#include "io/byte_stream.h"
#include "io/clock.h"
#include "ui/display.h"

class FakeClock final : public io::Clock {
public:
    FakeClock() : now(0) {}

    uint32_t nowMs() const override { return now; }

    void delayMs(uint32_t durationMs) override { now += durationMs; }

    void advance(uint32_t durationMs) { now += durationMs; }

    uint32_t now;
};

class FakeStream final : public io::ByteStream {
public:
    FakeStream()
        : started(false),
          failBegin(false),
          failWrite(false),
          inputOffset(0),
          responseOffset(0) {}

    bool begin() override {
        started = !failBegin;
        return started;
    }

    size_t write(const uint8_t* data, size_t size) override {
        if (!started || failWrite) {
            return 0;
        }
        output.insert(output.end(), data, data + size);
        if (responseOffset < responses.size()) {
            const std::vector<uint8_t>& response = responses[responseOffset++];
            input.insert(input.end(), response.begin(), response.end());
        }
        return size;
    }

    bool read(uint8_t& value) override {
        if (inputOffset == input.size()) {
            input.clear();
            inputOffset = 0;
            return false;
        }
        value = input[inputOffset++];
        return true;
    }

    void receive(std::initializer_list<uint8_t> data) {
        input.insert(input.end(), data.begin(), data.end());
    }

    void respondWith(std::initializer_list<uint8_t> data) {
        responses.emplace_back(data);
    }

    bool started;
    bool failBegin;
    bool failWrite;
    std::vector<uint8_t> input;
    size_t inputOffset;
    std::vector<uint8_t> output;
    std::vector<std::vector<uint8_t>> responses;
    size_t responseOffset;
};

class FakeDisplay final : public ui::Display {
public:
    bool begin() override { return true; }

    void clear(uint16_t) override { texts.clear(); }

    void text(uint16_t, uint16_t, const char* value, uint16_t,
              uint16_t) override {
        texts.emplace_back(value);
    }

    void integer(uint16_t, uint16_t, int32_t value, uint16_t,
                 uint16_t) override {
        texts.push_back(std::to_string(value));
    }

    void decimal(uint16_t, uint16_t, float value, uint16_t,
                 uint16_t) override {
        texts.push_back(std::to_string(value));
    }

    uint16_t width() const override { return 240; }

    uint16_t height() const override { return 135; }

    std::vector<std::string> texts;
};
