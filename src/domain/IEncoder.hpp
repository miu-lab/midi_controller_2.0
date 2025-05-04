#pragma once
#include "types.hpp"

class IEncoder {
public:
    virtual ~IEncoder() = default;
    virtual int8_t    readDelta()       = 0;
    virtual bool      isPressed() const = 0;
    virtual EncoderId getId()     const = 0;
    virtual uint16_t getPpr()     const  = 0;
};
