#include "cstarprotocolpc.h"
#include <QtGlobal>
#include <QtEndian>


#define CRC32_POLY   0x04C11DB7
#define CRC32_POLY_R 0xEDB88320


cStarProtocolPC &cStarProtocolPC::instance()
{
    static cStarProtocolPC s;
    return s;
}

uint32_t cStarProtocolPC::crc32stm32(uint8_t *bfr, uint8_t len, bool clear)
{
    return crc32Stm32Function(bfr, len, clear);
}

uint32_t cStarProtocolPC::crc32Stm32Function(uint8_t *bfr, uint8_t len, bool clear)
{
    if (clear) mCrc32_value = 0;
    quint32* buf;
    buf = (quint32*) bfr;
    quint32 v;
    quint32 crc;

    crc = ~mCrc32_value;
    while(len >= 4) {
        v = qToBigEndian(*buf++);
        crc = ( crc << 8 ) ^ mCrc32_table[0xFF & ( (crc >> 24) ^ (v ) )];
        crc = ( crc << 8 ) ^ mCrc32_table[0xFF & ( (crc >> 24) ^ (v >> 8) )];
        crc = ( crc << 8 ) ^ mCrc32_table[0xFF & ( (crc >> 24) ^ (v >> 16) )];
        crc = ( crc << 8 ) ^ mCrc32_table[0xFF & ( (crc >> 24) ^ (v >> 24) )];
        len -= 4;
    }
    if(len) {
        switch(len) {
        case 1: v = 0xFF000000 & qToBigEndian(*buf++); break;
        case 2: v = 0xFFFF0000 & qToBigEndian(*buf++); break;
        case 3: v = 0xFFFFFF00 & qToBigEndian(*buf++); break;
        default: v = 0;
        }
        crc = ( crc << 8 ) ^ mCrc32_table[0xFF & ( (crc >> 24) ^ (v ) )];
        crc = ( crc << 8 ) ^ mCrc32_table[0xFF & ( (crc >> 24) ^ (v >> 8) )];
        crc = ( crc << 8 ) ^ mCrc32_table[0xFF & ( (crc >> 24) ^ (v >> 16) )];
        crc = ( crc << 8 ) ^ mCrc32_table[0xFF & ( (crc >> 24) ^ (v >> 24) )];
    }
    mCrc32_value = ~crc;
    return mCrc32_value;
}

cStarProtocolPC::cStarProtocolPC():
    cAbstractStarProtocol(),
    mCrc32_value(0)
{
    int i, j;

    for (i = 0; i < 256; ++i) {
        uint32_t c = i << 24;
        uint32_t cr = i;
        for (j = 8; j > 0; --j) {
            c = (c & 0x80000000) ? (c << 1) ^ CRC32_POLY : (c << 1);
            cr = (cr & 0x00000001) ? (cr >> 1) ^ CRC32_POLY_R : (cr >> 1);
        }
        mCrc32_table[i] = c;
        mCrc32r_table[i] = cr;
    }
}
