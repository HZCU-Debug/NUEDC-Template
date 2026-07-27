#pragma once

#include "platform/system_clock.h"
#include "platform/uart.h"

namespace platform {

/**
 * @brief 获取系统单调时钟
 * @return 系统时钟
 */
SystemClock& systemClock();

/**
 * @brief 获取连接上位机的 UART0
 * @return 上位机通信 UART
 */
Uart& communicationUart();

/**
 * @brief 获取连接电机的 UART2
 * @return 电机通信 UART
 */
Uart& motorUart();

}
