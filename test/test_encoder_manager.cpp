// test/test_encoder_manager.cpp
#include <unity.h>
#ifdef UNIT_TEST_NATIVE
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif
#include <vector>
#include "interface_adapters/encoders/EncoderManager.hpp"
#include "interface_adapters/encoders/EncoderConfig.hpp"

void test_manager_creation() {
    std::vector<EncoderConfig> configs;
    EncoderConfig c1{}; c1.id=1; c1.pinA=2; c1.pinB=3;
    EncoderConfig c2{}; c2.id=2; c2.pinA=4; c2.pinB=5;
    configs.push_back(c1);
    configs.push_back(c2);
    EncoderManager mgr(configs);
    const auto& encs = mgr.getEncoders();
    TEST_ASSERT_EQUAL_size_t(2, encs.size());
    TEST_ASSERT_EQUAL_UINT8(1, encs[0]->getId());
    TEST_ASSERT_EQUAL_UINT8(2, encs[1]->getId());
}

void run_encoder_manager_tests() {
    RUN_TEST(test_manager_creation);
}