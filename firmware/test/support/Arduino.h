#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <initializer_list>
#include <vector>

#define SERIAL_8N1 0

inline unsigned long millis() {
    static unsigned long now = 0;
    return now++;
}

inline void delay(unsigned long) {}

class HardwareSerial {
public:
    void begin(unsigned long, uint32_t, int8_t, int8_t) {}

    size_t write(const uint8_t* data, size_t size) {
        transmitted.insert(transmitted.end(), data, data + size);
        if (!responses.empty()) {
            received.insert(received.end(), responses.front().begin(), responses.front().end());
            responses.pop_front();
        }
        return size;
    }

    int available() const { return static_cast<int>(received.size()); }

    int read() {
        if (received.empty()) {
            return -1;
        }
        const uint8_t value = received.front();
        received.pop_front();
        return value;
    }

    void flush() {}

    void respondWith(std::initializer_list<uint8_t> response) {
        responses.emplace_back(response);
    }

    std::vector<uint8_t> transmitted;

private:
    std::deque<uint8_t> received;
    std::deque<std::vector<uint8_t>> responses;
};
