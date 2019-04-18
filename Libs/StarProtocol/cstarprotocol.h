#ifndef CSTARPROTOCOL_H
#define CSTARPROTOCOL_H

//SingleTon
#include "stdint.h"
#include "star_prc_commands.h"

#define FakeSymbolShift 	(3)
#define START_PACKET		(0xF9) 							// start packet
#define END_PACKET			(0xFA) 							// end packet
#define SPECIAL_SYMBOL		(0xFB) 							// special symbol
#define FAKE_START_PACKET	(START_PACKET+FakeSymbolShift) 	// fake start packet
#define FAKE_END_PACKET		(END_PACKET+FakeSymbolShift)	// fake end packet
#define FAKE_SPECIAL_SYMBOL	(SPECIAL_SYMBOL+FakeSymbolShift)// fake special symbol

// Описание ошибок в работе протокола
#define NO_ERROR			0x00
#define PACKET_ERROR		0x01
#define CRC32_ERROR			0x02
#define UNKNOWN_COMMAND		0x03

typedef struct {
    char Address;
    char Command;
    char dataLength;
} StarHead_t;

typedef enum CRC32_INIT{
    CRC32_Not_Clear = false,
    CRC32_Clear = true
}CRC32_Init_Type;

class cAbstractStarProtocol
{
public:
    cAbstractStarProtocol();
    virtual ~cAbstractStarProtocol() = default;
    void clearBuffer();
    void codeData(char address, char command, char dataLength, char *data);
    uint8_t encodeBuffer();

    uint8_t *buffer();
    void addData(uint8_t *data, char dataLength);

    StarHead_t headPacket() const;

    uint8_t bufferLength() const;

protected:
    virtual uint32_t crc32Stm32Function(uint8_t *bfr, uint8_t len, bool clear) = 0;

private:
    uint8_t mBuffer[MaxBufferSize];
    StarHead_t mStarHead;
    uint8_t mStopBufferPosition;
    void codeByte(uint8_t byte);
    uint8_t nextPosition(uint8_t position);
    void addByteToBuffer(uint8_t byte);
};

#endif // CSTARPROTOCOL_H
