#include "TestFramework.hpp"

void TestSuite::addTest(const std::string& name, std::function<bool()> testFunction) {
    tests_.push_back({name, testFunction});
}

bool TestSuite::runAll() {
    bool allPassed = true;
    int passCount = 0;
    int failCount = 0;

    std::cout << "Running " << tests_.size() << " tests...\n";
    std::cout << "----------------------------------------\n";

    for (const auto& test : tests_) {
        std::cout << "Test: " << test.name << " ... ";
        bool result = test.function();
        if (result) {
            std::cout << "PASSED\n";
            passCount++;
        } else {
            std::cout << "FAILED\n";
            allPassed = false;
            failCount++;
        }
    }

    std::cout << "----------------------------------------\n";
    std::cout << "Results: " << passCount << " passed, " << failCount << " failed\n";
    
    return allPassed;
}
