#pragma once

#include <stddef.h>
#include <stdint.h>

#include "io/byte_stream.h"
#include "io/clock.h"

namespace zdt {

/**
 * @brief 电机操作失败原因
 */
enum class Error : uint8_t {
    None,
    NotStarted,
    InvalidArgument,
    WriteFailed,
    Timeout,
    InvalidResponse,
};

/**
 * @brief 不携带返回数据的操作结果
 */
struct Status {
    explicit Status(Error error = Error::None) : error(error) {}

    /**
     * @brief 判断操作是否成功
     * @return 成功时返回 true
     */
    explicit operator bool() const { return error == Error::None; }

    Error error;
};

/**
 * @brief 携带返回数据的操作结果
 * @tparam T 返回数据类型
 */
template <typename T>
struct Result {
    explicit Result(Error error = Error::None) : value(), error(error) {}
    explicit Result(const T& value) : value(value), error(Error::None) {}

    /**
     * @brief 判断操作是否成功
     * @return 成功时返回 true
     */
    explicit operator bool() const { return error == Error::None; }

    T value;
    Error error;
};

/**
 * @brief 命令执行方式
 */
enum class Start : uint8_t {
    Immediate = 0,
    Synchronized = 1,
};

/**
 * @brief 驱动器支持的回零方式
 */
enum class HomeMode : uint8_t {
    Nearest = 0,
    Directional = 1,
    Sensorless = 2,
    EndStop = 3,
};

/**
 * @brief 电机总线配置
 */
struct BusConfig {
    /**
     * @brief 创建电机总线配置
     * @param timeoutMs 查询应答超时
     */
    explicit BusConfig(uint32_t timeoutMs = 150) : timeoutMs(timeoutMs) {}

    uint32_t timeoutMs;
};

/**
 * @brief 单个电机逻辑配置
 */
struct MotorConfig {
    /**
     * @brief 创建电机逻辑配置
     * @param address 驱动器地址
     * @param pulsesPerRevolution 每圈脉冲数
     * @param invertDirection 是否反转逻辑方向
     */
    MotorConfig(uint8_t address, uint32_t pulsesPerRevolution,
                bool invertDirection = false)
        : address(address),
          pulsesPerRevolution(pulsesPerRevolution),
          invertDirection(invertDirection) {}

    uint8_t address;
    uint32_t pulsesPerRevolution;
    bool invertDirection;
};

/**
 * @brief 位置运动参数
 */
struct MotionOptions {
    /**
     * @brief 创建位置运动参数
     * @param rpm 最大转速
     * @param acceleration 加速度档位
     * @param start 命令执行方式
     */
    MotionOptions(uint16_t rpm, uint8_t acceleration = 0,
                  Start start = Start::Immediate)
        : rpm(rpm), acceleration(acceleration), start(start) {}

    uint16_t rpm;
    uint8_t acceleration;
    Start start;
};

/**
 * @brief 电机运行状态
 */
struct MotorState {
    MotorState()
        : enabled(false),
          reached(false),
          stalled(false),
          stallProtected(false),
          homing(false),
          homingFailed(false) {}

    bool enabled;
    bool reached;
    bool stalled;
    bool stallProtected;
    bool homing;
    bool homingFailed;
};

/**
 * @brief 一次读取获得的状态、位置和速度
 */
struct MotorSnapshot {
    MotorSnapshot() : state(), positionDegrees(0.0f), speedRpm(0.0f) {}

    MotorState state;
    float positionDegrees;
    float speedRpm;
};

class Motor;

/**
 * @brief 管理一条字节流上的电机收发、超时和同步触发
 */
class Bus {
public:
    /**
     * @brief 绑定电机字节流、时钟和总线配置
     * @param stream 电机通信字节流
     * @param clock 单调时钟
     * @param config 总线配置
     */
    Bus(io::ByteStream& stream, io::Clock& clock,
        const BusConfig& config = BusConfig());

    /**
     * @brief 初始化电机字节流
     * @return 初始化结果
     */
    Status begin();

    /**
     * @brief 广播触发所有等待同步命令的电机
     * @return 命令帧发送结果
     */
    Status triggerSynchronized();

private:
    friend class Motor;

    Status command(const uint8_t* frame, size_t size);
    Status query(uint8_t address, uint8_t function, const uint8_t* frame,
                 size_t frameSize, uint8_t* response, size_t responseSize);
    Status send(const uint8_t* frame, size_t size);
    Status receive(uint8_t address, uint8_t function, uint8_t* response,
                   size_t size);
    void discardInput();

    io::ByteStream& stream_;
    io::Clock& clock_;
    BusConfig config_;
    bool started_;
};

/**
 * @brief 使用角度、RPM 和状态控制一个地址上的电机
 */
class Motor {
public:
    /**
     * @brief 创建电机句柄
     * @param bus 电机总线
     * @param config 电机逻辑配置
     */
    Motor(Bus& bus, const MotorConfig& config);

    /**
     * @brief 使能或失能电机输出
     * @param enabled 是否使能
     * @return 命令帧发送结果
     */
    Status enable(bool enabled = true);

    /**
     * @brief 失能电机输出
     * @return 命令帧发送结果
     */
    Status disable() { return enable(false); }

    /**
     * @brief 以带符号 RPM 持续运行
     * @param signedRpm 目标转速，范围 -5000 至 5000 且不能为 0
     * @param acceleration 加速度档位
     * @param start 命令执行方式
     * @return 命令帧发送结果
     */
    Status run(int16_t signedRpm, uint8_t acceleration = 0,
               Start start = Start::Immediate);

    /**
     * @brief 按带符号角度相对移动
     * @param degrees 相对运动角度
     * @param options 位置运动参数
     * @return 命令帧发送结果
     */
    Status moveRelative(float degrees, const MotionOptions& options);

    /**
     * @brief 移动到带符号绝对角度
     * @param degrees 目标角度
     * @param options 位置运动参数
     * @return 命令帧发送结果
     */
    Status moveAbsolute(float degrees, const MotionOptions& options);

    /**
     * @brief 停止电机
     * @param start 命令执行方式
     * @return 命令帧发送结果
     */
    Status stop(Start start = Start::Immediate);

    /**
     * @brief 使用驱动器参数触发回零
     * @param mode 回零方式
     * @param start 命令执行方式
     * @return 命令帧发送结果
     */
    Status home(HomeMode mode = HomeMode::Nearest,
                Start start = Start::Immediate);

    /**
     * @brief 读取电机与回零状态
     * @return 电机状态或通信错误
     */
    Result<MotorState> readState();

    /**
     * @brief 读取带符号实时角度
     * @return 实时角度或通信错误
     */
    Result<float> readPositionDegrees();

    /**
     * @brief 读取带符号实时转速
     * @return 实时转速或通信错误
     */
    Result<float> readSpeedRpm();

    /**
     * @brief 顺序读取状态、位置和速度
     * @return 电机快照或通信错误
     */
    Result<MotorSnapshot> readSnapshot();

private:
    Status move(float degrees, const MotionOptions& options, bool absolute);
    bool valid() const;
    uint8_t directionFor(bool negative) const;

    Bus& bus_;
    MotorConfig config_;
};

}
