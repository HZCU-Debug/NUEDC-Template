/**
 * @file main.cpp
 * @brief Arduino 固件入口和当前 Demo 选择
 */
#include "demo/demo.h"

// 改这一行修改 demo
namespace selectedDemo = demo::commReliable;

void setup() { selectedDemo::setup(); }

void loop() { selectedDemo::loop(); }
