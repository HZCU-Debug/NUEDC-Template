#include "motor/system.h"

#include <Arduino.h>

#include "atk/motor.h"
#include "config/hardware.h"
#include "motor/atk_motor.h"
#include "motor/zdt_motor.h"
#include "zdt/motor.h"

extern HardwareSerial Serial2;

namespace motor {
namespace {

constexpr int8_t kMotorRxPins[] = {
    config::kPins.motorRx,
    config::kPins.motorYRx,
    config::kPins.motorZRx,
    config::kPins.motorRollRx,
};

constexpr int8_t kAtkTxPin = config::kPins.atkMotorTx >= 0
                                 ? config::kPins.atkMotorTx
                                 : config::kPins.motorTx;

template <Axis axis, config::MotorModel model>
struct ConfiguredMotor;

template <Axis axis>
struct ConfiguredMotor<axis, config::MotorModel::Zdt> {
    static Motor& get(zdt::Bus& zdtBus, atk::Bus&,
                      const config::AxisMotorConfig& config) {
        static ZdtMotor motor(
            zdtBus,
            zdt::MotorConfig(config.address, config.pulsesPerRevolution,
                             config.invertDirection));
        return motor;
    }
};

template <Axis axis>
struct ConfiguredMotor<axis, config::MotorModel::Atk> {
    static Motor& get(zdt::Bus&, atk::Bus& atkBus,
                      const config::AxisMotorConfig& config) {
        static AtkMotor motor(
            atkBus,
            atk::MotorConfig(config.address, config.invertDirection,
                             kMotorRxPins[static_cast<uint8_t>(axis)]));
        return motor;
    }
};

}

Motor& systemMotor(Axis axis) {
    static zdt::Bus zdtBus(
        Serial2, zdt::BusConfig(config::kMotorBaudRate,
                                config::kPins.motorRx,
                                config::kPins.motorTx));
    static atk::Bus atkBus(
        Serial2,
        atk::BusConfig(config::kMotorBaudRate, config::kPins.motorRx,
                       kAtkTxPin));
    static Motor& x =
        ConfiguredMotor<Axis::X, config::kMotorModel>::get(
            zdtBus, atkBus, config::kXMotor);
    static Motor& y =
        ConfiguredMotor<Axis::Y, config::kMotorModel>::get(
            zdtBus, atkBus, config::kYMotor);
    static Motor& z =
        ConfiguredMotor<Axis::Z, config::kMotorModel>::get(
            zdtBus, atkBus, config::kZMotor);
    static Motor& rotation =
        ConfiguredMotor<Axis::Rotation, config::kMotorModel>::get(
            zdtBus, atkBus, config::kRollMotor);

    switch (axis) {
        case Axis::X:
            return x;
        case Axis::Y:
            return y;
        case Axis::Z:
            return z;
        case Axis::Rotation:
            return rotation;
    }
    return x;
}

}
