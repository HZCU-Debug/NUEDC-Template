#pragma once

#include <stddef.h>
#include <stdint.h>

namespace io {

/**
 * @brief 提供协议模块所需的字节流收发能力
 */
class ByteStream {
public:
    /**
     * @brief 销毁字节流
     */
    virtual ~ByteStream() = default;

    /**
     * @brief 初始化底层字节流
     * @return 初始化是否成功
     */
    virtual bool begin() = 0;

    /**
     * @brief 写入连续字节
     * @param data 待写入数据
     * @param size 数据长度
     * @return 实际写入长度
     */
    virtual size_t write(const uint8_t* data, size_t size) = 0;

    /**
     * @brief 非阻塞读取一个字节
     * @param value 接收读取结果
     * @return 是否读取到字节
     */
    virtual bool read(uint8_t& value) = 0;
};

}
