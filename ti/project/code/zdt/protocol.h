#pragma once

#include <stddef.h>
#include <stdint.h>

#include "zdt/motor.h"

namespace zdt {
namespace protocol {

const size_t kMaxFrameSize = 13;

/**
 * @brief 驱动器查询命令
 */
enum class Query : uint8_t {
    Speed = 0x35,
    Position = 0x36,
    State = 0x3A,
    HomeState = 0x3B,
};

/**
 * @brief 待发送的 Emm_V5.0 命令帧
 */
struct Frame {
    uint8_t bytes[kMaxFrameSize];
    size_t size;
    uint8_t function;
};

/**
 * @brief 生成同步触发广播帧
 * @return 命令帧
 */
Frame synchronizedTrigger();

/**
 * @brief 生成使能或失能命令帧
 * @param address 驱动器地址
 * @param enabled 是否使能
 * @return 命令帧
 */
Frame enable(uint8_t address, bool enabled);

/**
 * @brief 生成速度运行命令帧
 * @param address 驱动器地址
 * @param direction 协议方向值
 * @param rpm 目标转速
 * @param acceleration 加速度档位
 * @param start 命令执行方式
 * @return 命令帧
 */
Frame run(uint8_t address, uint8_t direction, uint16_t rpm,
          uint8_t acceleration, Start start);

/**
 * @brief 生成位置运动命令帧
 * @param address 驱动器地址
 * @param direction 协议方向值
 * @param rpm 最大转速
 * @param acceleration 加速度档位
 * @param pulses 运动脉冲数
 * @param absolute 是否使用绝对位置
 * @param start 命令执行方式
 * @return 命令帧
 */
Frame move(uint8_t address, uint8_t direction, uint16_t rpm,
           uint8_t acceleration, uint32_t pulses, bool absolute, Start start);

/**
 * @brief 生成停止命令帧
 * @param address 驱动器地址
 * @param start 命令执行方式
 * @return 命令帧
 */
Frame stop(uint8_t address, Start start);

/**
 * @brief 生成回零命令帧
 * @param address 驱动器地址
 * @param mode 回零方式
 * @param start 命令执行方式
 * @return 命令帧
 */
Frame home(uint8_t address, HomeMode mode, Start start);

/**
 * @brief 生成状态查询命令帧
 * @param address 驱动器地址
 * @param query 查询类型
 * @return 命令帧
 */
Frame query(uint8_t address, Query query);

/**
 * @brief 判断响应是否为驱动器协议错误
 * @param response 响应字节
 * @param size 响应长度
 * @param address 驱动器地址
 * @return 协议错误时返回 true
 */
bool isProtocolError(const uint8_t* response, size_t size, uint8_t address);

/**
 * @brief 校验响应地址、功能码和固定校验字节
 * @param response 响应字节
 * @param size 响应长度
 * @param address 驱动器地址
 * @param function 预期功能码
 * @return 响应校验结果
 */
Error validateResponse(const uint8_t* response, size_t size, uint8_t address,
                       uint8_t function);

/**
 * @brief 将协议标志解析为电机状态
 * @param motorFlags 电机状态标志
 * @param homeFlags 回零状态标志
 * @return 电机状态
 */
MotorState motorState(uint8_t motorFlags, uint8_t homeFlags);

/**
 * @brief 将响应解析为带符号角度
 * @param response 位置响应
 * @param invertDirection 是否反转逻辑方向
 * @return 实时角度
 */
float positionDegrees(const uint8_t* response, bool invertDirection);

/**
 * @brief 将响应解析为带符号转速
 * @param response 速度响应
 * @param invertDirection 是否反转逻辑方向
 * @return 实时转速
 */
float speedRpm(const uint8_t* response, bool invertDirection);

}
}
