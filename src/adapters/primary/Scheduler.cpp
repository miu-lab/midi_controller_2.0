#include "Scheduler.hpp"

Scheduler::Scheduler(uint32_t interval_ms, Callback cb)
    : callback_(cb), interval_(interval_ms), next_due_(0) {}

void Scheduler::update(uint32_t now_ms) {
    if (now_ms >= next_due_) {
        callback_();
        next_due_ = now_ms + interval_;
    }
}
