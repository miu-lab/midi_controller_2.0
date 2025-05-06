#pragma once
#include "types.hpp"

class IEncoder {
public:
    virtual ~IEncoder() = default;
    virtual int8_t    readDelta()               = 0;
    virtual bool      isPressed() const         = 0;
    virtual EncoderId getId()     const         = 0;
    virtual uint16_t  getPpr()    const         = 0;
    
    // Méthodes pour la position absolue
    virtual int32_t   getAbsolutePosition() const = 0;
    virtual int32_t   getPhysicalPosition() const = 0;
    virtual void      resetPosition()             = 0;
};
