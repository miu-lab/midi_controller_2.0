#include <unity.h>
#include "utils/EventBus.hpp"

struct Dummy {
    int v;
};

static int sum = 0;

static void dummyCallback(const Dummy& d) {
    sum += d.v;
}

void test_event_publish() {
    sum = 0;  // réinitialiser avant chaque test

    EventBus<Dummy>::subscribe(dummyCallback);
    EventBus<Dummy>::publish({42});

    TEST_ASSERT_EQUAL(42, sum);
}

void run_event_bus_tests() {
    RUN_TEST(test_event_publish);
}
