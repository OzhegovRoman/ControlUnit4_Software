#include "cstarprotocolstm32.h"

uint32_t cStarProtocolStm32::crc32Stm32Function(uint8_t *bfr, uint8_t len, bool clear)
{
    return crc32Stm32(bfr, len, clear, &mHcrc);
}

cStarProtocolStm32::cStarProtocolStm32()
    : mHcrc {}
{
    mHcrc.Instance = CRC;
    HAL_CRC_Init(&mHcrc);
    __HAL_RCC_CRC_CLK_ENABLE();
}

uint32_t crc32Stm32(uint8_t *bfr, uint8_t len, bool clear, CRC_HandleTypeDef *HCrc)
{
    uint32_t l;
    uint32_t *p, x, crc;
    l = len / 4;
    p = reinterpret_cast<uint32_t*>(bfr);
    x = p[l];
    if (clear)
        crc = HAL_CRC_Calculate(HCrc, p, l);
    else
        crc = HAL_CRC_Accumulate(HCrc, p, l);

    switch(len & 3) {
    case 1:
        x &= 0x000000FF;
        break;
    case 2:
        x &= 0x0000FFFF;
        break;
    case 3:
        x &= 0x00FFFFFF;
        break;
    }
    if (len%4)
        crc = HAL_CRC_Accumulate(HCrc, &x, 1);

    return 0xFFFFFFFF ^ crc;
}
