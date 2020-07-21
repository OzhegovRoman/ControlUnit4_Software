#ifndef CRELAYSSTATUS_H
#define CRELAYSSTATUS_H

#include <stdint.h>

class cRelaysStatus
{
public:
    enum RelayIndex{
        ri25V = 0b01,
        ri5V = 0b10
    };

    cRelaysStatus();
    cRelaysStatus(RelayIndex mask, RelayIndex value);

    uint8_t status() const;
    void setStatus(const uint8_t &value);
    void setStatus(const uint8_t &mask, const uint8_t& value);

    bool operator[](int index) const;
    cRelaysStatus& operator=(const uint8_t&value);

    void clearMask();
    void setValue(uint8_t index, bool value);
private:
    uint8_t mStatus;
};


#endif // CRELAYSSTATUS_H
