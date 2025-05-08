#pragma once
#include "core/domain/IEncoder.hpp"
#include <vector>

class ProcessEncoders {
public:
    explicit ProcessEncoders(const std::vector<IEncoder*>& encoders);
    void update();

private:
    std::vector<IEncoder*> encoders_;
    std::vector<bool> lastPressed_;
};
