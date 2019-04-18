#ifndef AOINTERFACE_H
#define AOINTERFACE_H

#include "stdint.h"

class AbstractIOInterface
{
public:
    virtual ~AbstractIOInterface() = 0;
    virtual void sendData(const uint8_t address, const uint8_t command, const uint8_t dataLen, const uint8_t *data);
};
#endif // AOINTERFACE_H




