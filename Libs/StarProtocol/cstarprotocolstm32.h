#ifndef CSTARPROTOCOLSTM32_H
#define CSTARPROTOCOLSTM32_H

#include "stdint.h"
#include "cstarprotocol.h"
#include "stm32f1xx_hal.h"

extern "C" uint32_t crc32Stm32(uint8_t *bfr, uint8_t len, bool clear, CRC_HandleTypeDef *HCrc);

class cStarProtocolStm32 : public cAbstractStarProtocol
{
public:
    cStarProtocolStm32();
    virtual ~cStarProtocolStm32() = default;

protected:
    uint32_t crc32Stm32Function(uint8_t *bfr, uint8_t len, bool clear);

private:
    CRC_HandleTypeDef mHcrc;
};


#endif // CSTARPROTOCOLSTM32_H
