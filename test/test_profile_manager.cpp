#include <unity.h>
#include "storage/ProfileManager.hpp"

void test_profile_manager_basic() {
    ProfileManager manager;
    ControlId id = 7;
    MidiControl binding{1, 42, true};
    
    manager.setBinding(id, binding);

    auto loaded = manager.getBinding(id);
    TEST_ASSERT_TRUE(loaded.has_value());
    TEST_ASSERT_EQUAL(1, loaded->channel);
    TEST_ASSERT_EQUAL(42, loaded->control);
    TEST_ASSERT_TRUE(loaded->relative);
}

void run_profile_manager_tests() {
    RUN_TEST(test_profile_manager_basic);
}
