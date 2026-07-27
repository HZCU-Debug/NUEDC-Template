#pragma once

#include "io/byte_stream.h"
#include "platform/zf.h"

namespace platform {

/**
 * @brief 使用逐飞轮询接口实现一个硬件 UART 字节流
 */
class Uart final : public io::ByteStream {
public:
    /**
     * @brief 保存 UART 及引脚配置
     * @param index UART 外设编号
     * @param baudRate 波特率
     * @param txPin 发送引脚
     * @param rxPin 接收引脚
     */
    Uart(uart_index_enum index, uint32_t baudRate, uart_tx_pin_enum txPin,
         uart_rx_pin_enum rxPin);

    /**
     * @brief 初始化 UART 并清空已有输入
     * @return 配置有效时返回 true
     */
    bool begin() override;

    /**
     * @brief 阻塞写入连续字节
     * @param data 待写入数据
     * @param size 数据长度
     * @return 实际写入长度
     */
    size_t write(const uint8_t* data, size_t size) override;

    /**
     * @brief 查询并读取一个字节
     * @param value 接收读取结果
     * @return 是否读取到字节
     */
    bool read(uint8_t& value) override;

private:
    static const uint16_t kReceiveCapacity = 128;

    static void receiveCallback(uint32 event, void* context);
    void bufferInput();

    uart_index_enum index_;
    uint32_t baudRate_;
    uart_tx_pin_enum txPin_;
    uart_rx_pin_enum rxPin_;
    bool started_;
    uint8_t receiveBuffer_[kReceiveCapacity];
    volatile uint16_t receiveHead_;
    volatile uint16_t receiveTail_;
};

}
