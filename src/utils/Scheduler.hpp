#pragma once
#include <cstdint>

class Scheduler {
public:
    using Callback = void (*)();

    Scheduler(uint32_t interval_ms, Callback cb);
    void update(uint32_t now_ms);

private:
    Callback callback_;
    uint32_t interval_;
    uint32_t next_due_;
};
