#include "imu/gyroscope.h"

#include "platform/zf.h"

namespace gyroscope {
namespace {

const float kDegreesToRadians = 0.01745329251994329577f;
const float kGyroBiasXDps = 0.0f;
const float kGyroBiasYDps = 0.0f;
const float kGyroBiasZDps = 0.0f;

uint16_t lastSampleAt = 0;

}

bool begin() {
    const bool ready = imu660rb_init() == 0;
    timer_init(TIM_G7, TIMER_US);
    DL_Timer_setCounterRepeatMode(TIMG7, DL_TIMER_REPEAT_MODE_ENABLED);
    timer_start(TIM_G7);
    lastSampleAt = timer_get(TIM_G7);
    return ready;
}

bool read(Sample& sample) {
    const uint16_t sampledAt = timer_get(TIM_G7);
    // ponytail: 16 位微秒计时要求相邻采样间隔小于 65 ms
    const uint16_t elapsedMicros =
        static_cast<uint16_t>(sampledAt - lastSampleAt);
    if (elapsedMicros == 0) {
        return false;
    }
    lastSampleAt = sampledAt;

    imu660rb_get_gyro();
    sample.gyroX =
        (imu660rb_gyro_transition(imu660rb_gyro_x) - kGyroBiasXDps) *
        kDegreesToRadians;
    sample.gyroY =
        (imu660rb_gyro_transition(imu660rb_gyro_y) - kGyroBiasYDps) *
        kDegreesToRadians;
    sample.gyroZ =
        (imu660rb_gyro_transition(imu660rb_gyro_z) - kGyroBiasZDps) *
        kDegreesToRadians;
    sample.dtSeconds = static_cast<float>(elapsedMicros) / 1000000.0f;
    return true;
}

void end() { timer_stop(TIM_G7); }

}
