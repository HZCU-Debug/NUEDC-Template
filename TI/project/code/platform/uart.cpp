#include "platform/uart.h"

namespace platform {

Uart::Uart(uart_index_enum index, uint32_t baudRate, uart_tx_pin_enum txPin,
           uart_rx_pin_enum rxPin)
    : index_(index),
      baudRate_(baudRate),
      txPin_(txPin),
      rxPin_(rxPin),
      started_(false),
      receiveBuffer_(),
      receiveHead_(0),
      receiveTail_(0) {}

bool Uart::begin() {
    if (baudRate_ == 0) {
        return false;
    }
    if (started_) {
        uart_set_interrupt_config(index_, UART_INTERRUPT_CONFIG_RX_DISABLE);
    }
    uart_init(index_, baudRate_, txPin_, rxPin_);
    receiveHead_ = 0;
    receiveTail_ = 0;
    started_ = true;
    uart_set_callback(index_, receiveCallback, this);
    uart_set_interrupt_config(index_, UART_INTERRUPT_CONFIG_RX_ENABLE);
    return true;
}

size_t Uart::write(const uint8_t* data, size_t size) {
    if (!started_ || (data == nullptr && size != 0)) {
        return 0;
    }
    if (size != 0) {
        uart_write_buffer(index_, data, static_cast<uint32>(size));
    }
    return size;
}

bool Uart::read(uint8_t& value) {
    if (!started_ || receiveTail_ == receiveHead_) {
        return false;
    }
    value = receiveBuffer_[receiveTail_];
    receiveTail_ = static_cast<uint16_t>((receiveTail_ + 1) % kReceiveCapacity);
    return true;
}

void Uart::receiveCallback(uint32, void* context) {
    static_cast<Uart*>(context)->bufferInput();
}

void Uart::bufferInput() {
    uint8_t value = 0;
    while (uart_query_byte(index_, &value) == ZF_TRUE) {
        const uint16_t next =
            static_cast<uint16_t>((receiveHead_ + 1) % kReceiveCapacity);
        if (next != receiveTail_) {
            receiveBuffer_[receiveHead_] = value;
            receiveHead_ = next;
        }
    }
}

}
