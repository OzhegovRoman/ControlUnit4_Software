#ifndef AOINTERFACE_H
#define AOINTERFACE_H

#include "stdint.h"

class AbstractIOInterface
{
public:
    virtual ~AbstractIOInterface() = default;
    virtual void sendData(const uint8_t address, const uint8_t command, const uint8_t dataLen, const uint8_t *data) = 0;
};
#endif // AOINTERFACE_H




