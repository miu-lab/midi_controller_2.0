#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <vector>

class TestSuite {
public:
    void addTest(const std::string& name, std::function<bool()> testFunction);
    bool runAll();

private:
    struct Test {
        std::string name;
        std::function<bool()> function;
    };
    std::vector<Test> tests_;
};

#define TEST_ASSERT(condition) if (!(condition)) { \
    std::cerr << "Assertion failed: " << #condition << " in " << __FILE__ << " line " << __LINE__ << std::endl; \
    return false; \
}

#define REGISTER_TEST(suite, name, function) suite.addTest(name, function)
