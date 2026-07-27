#include <cassert>
#include <vector>

#include "platform/uart.h"
#include "platform/zf.h"

int main() {
    uart_fake::reset();
    platform::Uart uart(UART_2, 115200, UART2_TX_B15, UART2_RX_B16);
    assert(uart.begin());

    const uint8 response[] = {0x01, 0x36, 0x00, 0x00,
                              0x00, 0x40, 0x00, 0x6B};
    uart_fake::receive(response, sizeof(response));

    std::vector<uint8_t> received;
    uint8_t byte = 0;
    while (uart.read(byte)) {
        received.push_back(byte);
    }
    assert(received == std::vector<uint8_t>(response, response + 8));
}
