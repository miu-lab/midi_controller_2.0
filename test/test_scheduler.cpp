#include <unity.h>
#include "utils/Scheduler.hpp"

static int ticksA = 0;
static int ticksB = 0;

void tickA() { ticksA++; }
void tickB() { ticksB++; }

void test_scheduler_timing() {
    Scheduler s1(10, tickA);
    Scheduler s2(15, tickB);

    for (uint32_t t = 0; t <= 30; ++t) {
        s1.update(t);
        s2.update(t);
    }

    TEST_ASSERT_EQUAL(4, ticksA); // 0,10,20,30
    TEST_ASSERT_EQUAL(3, ticksB); // 0,15,30
}

void run_scheduler_tests() {
    RUN_TEST(test_scheduler_timing);
}
