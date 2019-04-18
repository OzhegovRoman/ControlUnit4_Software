#ifndef CSTARPROTOCOLPC_H
#define CSTARPROTOCOLPC_H

#include "cstarprotocol.h"
#include "stdint.h"

class cStarProtocolPC : public cAbstractStarProtocol
{
public:
    static cStarProtocolPC& instance();
    uint32_t crc32stm32(uint8_t *bfr, uint8_t len, bool clear);
protected:
    uint32_t crc32Stm32Function(uint8_t *bfr, uint8_t len, bool clear);

private:
    cStarProtocolPC();
    ~cStarProtocolPC(){}
    cStarProtocolPC(cStarProtocolPC const&);
    cStarProtocolPC& operator= (cStarProtocolPC const&);

    uint32_t mCrc32_table[256];
    uint32_t mCrc32r_table[256];
    uint32_t mCrc32_value;

};

#endif // CSTARPROTOCOLPC_H
