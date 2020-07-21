#include "crelaysstatus.h"
#include <cassert>

cRelaysStatus::cRelaysStatus():
    mStatus(0)
{

}

cRelaysStatus::cRelaysStatus(cRelaysStatus::RelayIndex mask, cRelaysStatus::RelayIndex value)
{
    setStatus(mask, value);
}

uint8_t cRelaysStatus::status() const
{
    return mStatus;
}

void cRelaysStatus::setStatus(const uint8_t &value)
{
    mStatus = value;
}

void cRelaysStatus::setStatus(const uint8_t &mask, const uint8_t &value)
{
    setStatus((mask << 4) + value);
}

bool cRelaysStatus::operator[](int index) const
{
    assert(index < 2 && "relay index must be < 2");
    return mStatus & (1<<index);
}

cRelaysStatus& cRelaysStatus::operator=(const uint8_t &value)
{
    setStatus(value);
    return *this;
}

void cRelaysStatus::clearMask()
{
    mStatus &= 0x0f;
}

void cRelaysStatus::setValue(uint8_t index, bool value)
{
    assert(index < 2);
    mStatus |= ( 1 << (index + 4));
    if (value)
        mStatus |= 1<<index;
    else
        mStatus &= ~(1<<index);
}
