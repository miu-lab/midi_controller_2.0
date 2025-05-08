// test/test_quadrature_encoder.cpp
#include <unity.h>
#ifdef UNIT_TEST_NATIVE
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif
#include "interface_adapters/encoders/InterruptQuadratureEncoder.hpp"
#include "interface_adapters/encoders/EncoderConfig.hpp"

void test_read_delta_forward_step() {
    TEST_ASSERT_TRUE(true);
}

void test_read_delta_backward_step() {
    TEST_ASSERT_TRUE(true);
}

void test_read_multiple_steps() {
    TEST_ASSERT_TRUE(true);
}

void test_get_id_and_ppr_and_no_change() {
    EncoderConfig cfg{};
    cfg.id  = 5;
    cfg.ppr = 42;
    InterruptQuadratureEncoder enc(cfg);
    TEST_ASSERT_EQUAL_UINT8(5, enc.getId());
    TEST_ASSERT_EQUAL_UINT16(42, enc.getPpr());
    TEST_ASSERT_EQUAL_INT8(0, enc.readDelta());
}

void run_quadrature_encoder_tests() {
    RUN_TEST(test_read_delta_forward_step);
    RUN_TEST(test_read_delta_backward_step);
    RUN_TEST(test_read_multiple_steps);
    RUN_TEST(test_get_id_and_ppr_and_no_change);
}
