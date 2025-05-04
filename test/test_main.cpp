// test/tests_main.cpp
#include <unity.h>

#include <Arduino.h>




extern void test_read_delta_forward_step();
extern void test_read_delta_backward_step();
extern void test_read_multiple_steps();
extern void test_get_id_and_ppr_and_no_change();
extern void test_manager_creation();
extern void run_event_bus_tests();
extern void run_input_router_tests();
extern void run_midi_buffer_tests();
extern void run_process_buttons_tests();
extern void run_profile_manager_tests();
extern void run_scheduler_tests();



void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_read_delta_forward_step);
    RUN_TEST(test_read_delta_backward_step);
    RUN_TEST(test_read_multiple_steps);
    RUN_TEST(test_get_id_and_ppr_and_no_change);
    RUN_TEST(test_manager_creation);

    run_scheduler_tests();
    run_input_router_tests();
    run_midi_buffer_tests();
    run_process_buttons_tests();
    run_profile_manager_tests();
    run_event_bus_tests();
    UNITY_END();
}

void loop() {}
