/*****************************************************************************
* Copyright (c) Future Technology Devices International 2014
* propriety of Future Technology devices International.
*
* Software License Agreement
*
* This code is provided as an example only and is not guaranteed by FTDI. 
* FTDI accept no responsibility for any issues resulting from its use. 
* The developer of the final application incorporating any parts of this 
* sample project is responsible for ensuring its safe and correct operation 
* and for any consequences resulting from its use.
*****************************************************************************/
/**
* @file                           FT_Transport_SPI.h
* @brief                          Contains FT transport layer specific to SPI for arduino platform.
                                  Tested platform version: Arduino 1.0.4 and later
                                  Dependencies: arduino standard SPI library
* @version                        1.1.0
* @date                           2014/02/05
*
*/

#ifndef _FT_TRANSPORT_SPI_H_
#define _FT_TRANSPORT_SPI_H_

//#include "SPI.h"
#include "stm32f1xx_hal.h"
#include <string.h>

extern SPI_HandleTypeDef hspi2;


inline void reorder16bit(const uint8_t * src, uint8_t * dst)
{
    uint16_t value = *(uint16_t*)src;
    *(uint16_t*)dst = value >> 8 | value << 8;
}
inline void reorder16bit2(const uint8_t * src, uint8_t * dst)
{
    uint32_t value = *(uint32_t*)src;
    *(uint32_t*)dst = (value & 0xFF00FF00) >> 8 | (value & 0x00FF00FF) << 8;
}
inline void reorder32bit(const uint32_t * src, uint32_t * dst)
{
    uint32_t value = *(uint32_t*)src;
    *(uint32_t*)dst = (value & 0x00FF0000) >> 8  | (value & 0x0000FF00) << 8
                                              | (value & 0xFF000000) >> 24 | (value & 0x000000FF) << 24;
}

class FT_Transport_SPI
{
public:
    /* Constructor and destructor methods */
    //initialize spi library
    FT_Transport_SPI()
    {
        //CSPin = cspin; //new library interface
    }

    ~FT_Transport_SPI(void){}//exit spi library
    void Init()
    {
        /* Initialize SPI library */
        // Для STM32 все уже реализовано
        HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, GPIO_PIN_SET);
    }

    void Exit()
    {
        /* close the spi channel */
        // ничего не делаем тут
    }

    void ChangeClock(uint32_t ClockValue)
    {
        /* For due it is straight forward, for others only a switch */
        // ничего не делаем тут. Оставляем заглушку
        UNUSED(ClockValue);
    }

    void HostCommand(uint8_t cmd)
    {
        uint8_t A[3] = {0};
        A[0] = cmd;
        HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(&hspi2, A, 3, 0);
        HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, GPIO_PIN_SET);
    }

    /* APIs related to memory read & write/transport */
    void StartRead(uint32_t Addr)
    {
        //make sure the union array indexes are modified for big endian usecase
        uint8_t  A[4];
        Addr <<= 8;
        reorder32bit(&Addr, (uint32_t*)A);
        HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(&hspi2, A, 4, 0);
    }

    void StartWrite(uint32_t Addr)
    {
        uint8_t  A[4];
        Addr <<= 8;
        reorder32bit(&Addr, (uint32_t*)A);
        A[0] |= 0x80;
        HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(&hspi2, A, 3, 0);
    }

    //Read a byte from Addr location
    uint8_t Read(uint32_t Addr)
    {
        uint8_t ReadByte = 0;
        StartRead(Addr);
        HAL_SPI_Receive(&hspi2, &ReadByte, 1, 0);
        EndTransfer();
        return (ReadByte);
    }

    //read a short from Addr location
    uint16_t Read16(uint32_t Addr)
    {
        uint16_t ReadWord = 0;
        StartRead(Addr);
        HAL_SPI_Receive(&hspi2, (uint8_t*)&ReadWord, 2, 0);
        EndTransfer();
        return (ReadWord);
    }
    //read a word from Addr location
    uint32_t Read32(uint32_t Addr)
    {
        uint32_t ReadInt = 0;
        StartRead(Addr);
        HAL_SPI_Receive(&hspi2, (uint8_t*)&ReadInt, 4, 0);
        EndTransfer();
        return (ReadInt);
    }
    //read N bytes from Addr location
    void Read(uint32_t Addr, uint8_t *Src, uint32_t NBytes)
    {

        // NOTE: На всякий случай обнуляю память, в случае потом удалить
        memset(Src, 0, NBytes);
        StartRead(Addr);
        HAL_SPI_Receive(&hspi2, Src, NBytes, 0);
        EndTransfer();
    }

    //write a byte to Addr location
    void Write(uint32_t Addr, uint8_t Value8)
    {
        StartWrite(Addr);
        HAL_SPI_Transmit(&hspi2, &Value8, 1, 0);
        EndTransfer();
    }
    //write a short to Addr location
    void Write16(uint32_t Addr, uint16_t Value16)
    {
        ///little endian read
        StartWrite(Addr);
        HAL_SPI_Transmit(&hspi2, (uint8_t*) &Value16, 2, 0);
        EndTransfer();
    }
    //write a word to Addr location
    void Write32(uint32_t Addr, uint32_t Value32)
    {
        StartWrite(Addr);
        HAL_SPI_Transmit(&hspi2, (uint8_t*) &Value32, 4, 0);
        EndTransfer();
    }

    void Write(uint32_t Addr, uint8_t *Src, uint32_t NBytes)
    {
        StartWrite(Addr);
        HAL_SPI_Transmit(&hspi2, Src, NBytes, 0);
        EndTransfer();
    }

    // у меня вроде как нет Flash
    void Writefromflash(uint32_t Addr, uint8_t *Src, uint32_t NBytes)
    {
        Write(Addr, Src, NBytes);
    }

    //apis useful for performance via spi - write only functionality
    //assert CSpin and send write command
    // не вижу отличия от StartWrite
    void StartTransfer(uint32_t Addr)
    {
        StartWrite(Addr);
    }

    //de assert CSpin
    void EndTransfer(void)
    {
        HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, GPIO_PIN_SET);
    }
    //transfer a single byte
    void Transfer(uint8_t Value8)
    {
        HAL_SPI_Transmit(&hspi2, &Value8, 1, 0);
    }
    //transfer 2 bytes
    void Transfer16(uint16_t Value16)
    {
        HAL_SPI_Transmit(&hspi2, (uint8_t*)&Value16, 2, 0);
    }
    //transfer 4 bytes
    void Transfer32(uint32_t Value32)
    {
        HAL_SPI_Transmit(&hspi2, (uint8_t*)&Value32, 4, 0);
    }
    //transfer N bytes
    void Transfer(uint8_t *Buff,uint32_t NBytes)
    {
        HAL_SPI_Transmit(&hspi2, Buff, NBytes, 0);
    }
};


#endif /* _FT_TRANSPORT_SPI_H_ */

