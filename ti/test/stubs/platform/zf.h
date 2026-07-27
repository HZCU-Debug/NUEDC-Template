#pragma once

#include <cstddef>
#include <cstdint>

using uint16 = std::uint16_t;
using int32 = std::int32_t;
using uint8 = std::uint8_t;
using uint32 = std::uint32_t;

enum ips114_dir_enum {
    IPS114_CROSSWISE,
};

enum ips114_font_size_enum {
    IPS114_8X16_FONT,
};

constexpr uint16 RGB565_BLACK = 0;
constexpr uint8 ZF_TRUE = 1;
constexpr uint8 ZF_FALSE = 0;

enum uart_index_enum {
    UART_0,
    UART_1,
    UART_2,
    UART_3,
};

enum uart_tx_pin_enum {
    UART2_TX_B15,
};

enum uart_rx_pin_enum {
    UART2_RX_B16,
};

enum uart_interrupt_config_enum {
    UART_INTERRUPT_CONFIG_RX_DISABLE,
    UART_INTERRUPT_CONFIG_RX_ENABLE,
};

using uart_callback = void (*)(uint32, void*);

namespace uart_fake {

inline uart_callback callback = nullptr;
inline void* callbackContext = nullptr;
inline bool interruptEnabled = false;
inline bool byteAvailable = false;
inline uint8 hardwareByte = 0;

inline void reset() {
    callback = nullptr;
    callbackContext = nullptr;
    interruptEnabled = false;
    byteAvailable = false;
    hardwareByte = 0;
}

inline void receive(const uint8* data, std::size_t size) {
    for (std::size_t index = 0; index < size; ++index) {
        hardwareByte = data[index];
        byteAvailable = true;
        if (interruptEnabled && callback != nullptr) {
            callback(0, callbackContext);
        }
    }
}

}

namespace ips114_fake {

inline unsigned initCalls = 0;
inline unsigned setDirCalls = 0;
inline unsigned integerDigits = 0;
inline unsigned decimalIntegerDigits = 0;
inline unsigned decimalFractionDigits = 0;

inline void reset() {
    initCalls = 0;
    setDirCalls = 0;
    integerDigits = 0;
    decimalIntegerDigits = 0;
    decimalFractionDigits = 0;
}

}

extern "C" inline void ips114_init() { ++ips114_fake::initCalls; }

extern "C" inline void ips114_set_dir(ips114_dir_enum) {
    ++ips114_fake::setDirCalls;
}

extern "C" inline void ips114_set_font(ips114_font_size_enum) {}
extern "C" inline void ips114_full(uint16) {}
extern "C" inline void ips114_set_color(uint16, uint16) {}
extern "C" inline void ips114_show_string(uint16, uint16, const char*) {}

extern "C" inline void ips114_show_int(uint16, uint16, int32, std::uint8_t num) {
    ips114_fake::integerDigits = num;
}

extern "C" inline void ips114_show_float(uint16, uint16, double,
                                          std::uint8_t num,
                                          std::uint8_t pointnum) {
    ips114_fake::decimalIntegerDigits = num;
    ips114_fake::decimalFractionDigits = pointnum;
}

extern "C" inline void uart_init(uart_index_enum, uint32, uart_tx_pin_enum,
                                  uart_rx_pin_enum) {}

extern "C" inline void uart_write_buffer(uart_index_enum, const uint8*,
                                          uint32) {}

extern "C" inline uint8 uart_query_byte(uart_index_enum, uint8* value) {
    if (!uart_fake::byteAvailable) {
        return ZF_FALSE;
    }
    *value = uart_fake::hardwareByte;
    uart_fake::byteAvailable = false;
    return ZF_TRUE;
}

extern "C" inline void uart_set_callback(uart_index_enum,
                                          uart_callback callback,
                                          void* context) {
    uart_fake::callback = callback;
    uart_fake::callbackContext = context;
}

extern "C" inline void uart_set_interrupt_config(
    uart_index_enum, uart_interrupt_config_enum config) {
    uart_fake::interruptEnabled = config == UART_INTERRUPT_CONFIG_RX_ENABLE;
}
