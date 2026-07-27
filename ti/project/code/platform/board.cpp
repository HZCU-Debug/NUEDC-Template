#include "platform/board.h"

namespace platform {

SystemClock& systemClock() {
    static SystemClock clock;
    return clock;
}

Uart& communicationUart() {
    static Uart uart(UART_0, 115200, UART0_TX_A10, UART0_RX_A11);
    return uart;
}

Uart& motorUart() {
    static Uart uart(UART_2, 115200, UART2_TX_B15, UART2_RX_B16);
    return uart;
}

}
