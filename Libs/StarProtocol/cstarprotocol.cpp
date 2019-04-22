#include "cstarprotocol.h"

cAbstractStarProtocol::cAbstractStarProtocol()
    : mBuffer {}
    , mStarHead {}
{

}

StarHead_t cAbstractStarProtocol::headPacket() const
{
    return mStarHead;
}

uint8_t cAbstractStarProtocol::bufferLength() const
{
    return mStopBufferPosition;
}

void cAbstractStarProtocol::codeByte(uint8_t byte)
{
    if ((byte == START_PACKET) || (byte == END_PACKET) || (byte == SPECIAL_SYMBOL)){
        addByteToBuffer(SPECIAL_SYMBOL);
        byte += FakeSymbolShift;
    }
    addByteToBuffer(byte);
}

void cAbstractStarProtocol::addByteToBuffer(uint8_t byte)
{
    mBuffer[mStopBufferPosition++] = byte;
}

void cAbstractStarProtocol::clearBuffer()
{
    mStopBufferPosition = 0;
}

void cAbstractStarProtocol::codeData(char address, char command, char dataLength, char *data)
{
    // используем tmp для расчета CRC32
    uint8_t tmp[4];
    tmp[0] = address;
    tmp[1] = command;
    tmp[2] = dataLength;
    if (dataLength) tmp[3] = data[0];
    unsigned int CRC32 = crc32Stm32Function(tmp, dataLength ? 4 : 3, CRC32_Clear);
    if (dataLength > 1)
        CRC32 = crc32Stm32Function((uint8_t *)data+1, dataLength - 1, CRC32_Not_Clear);

    char* tmpCRC = (char*)&CRC32;

    // а теперь начинаем вбивать буфер побайтно
    //    mStartBufferPosition = mStopBufferPosition;
    clearBuffer();
    addByteToBuffer(START_PACKET);
    codeByte(address);                       // адрес устройства
    codeByte(command);                       // команда
    codeByte(dataLength);                    // размер данных
    while (dataLength--) codeByte(*data++);  //сами данные
    uint8_t i;
    for (i = 0; i<4; i++)
        // выделяем правильный байт
        codeByte(tmpCRC[i]);
    addByteToBuffer(END_PACKET);
}

uint8_t cAbstractStarProtocol::encodeBuffer()
{
    uint8_t* dest = mBuffer;
    uint8_t* source = dest+1;
    // проверяем начало пакета
    // Делается обратный byte_staffing
    if (*dest != START_PACKET) return PACKET_ERROR;
    while (*source!=END_PACKET) //TODO: сделать проверку на выход за пределы буфера
    {
        if (*source == SPECIAL_SYMBOL){
            ++source;
            if ((*source == FAKE_START_PACKET) || (*source == FAKE_END_PACKET) || (*source == FAKE_SPECIAL_SYMBOL))
                *source -= FakeSymbolShift;
            else
                return PACKET_ERROR;
        }
        *dest++ = *source++;
    }

    // Проверяется соответствие CRC
    int len = dest-mBuffer;
    unsigned int result = crc32Stm32Function(mBuffer, len-4, (int)CRC32_Clear);
    if (result != *((unsigned int*) (mBuffer+len-4)))  return CRC32_ERROR;

    // Заполняется заголовок пакета
    source = dest = mBuffer;
    mStarHead.Address = *source++;
    mStarHead.Command = *source++;
    mStarHead.dataLength = *source++;

    // Данные помещаются в начало буфера
    int i;
    for (i = 0; i < mStarHead.dataLength; i++)
        *dest++ = *source++;

    return NO_ERROR;
}

uint8_t *cAbstractStarProtocol::buffer()
{
    return mBuffer;
}

void cAbstractStarProtocol::addData(uint8_t *data, char dataLength)
{
    while (dataLength--) mBuffer[mStopBufferPosition++] = *data++;
}

