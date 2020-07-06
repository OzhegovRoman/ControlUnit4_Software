/*
 * Copyright (c) Bridgetek Pte Ltd
 * Copyright (c) Riverdi Sp. z o.o. sp. k. <riverdi@riverdi.com>
 * Copyright (c) Skalski Embedded Technologies <contact@lukasz-skalski.com>
 *
 * THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL BRIDGETEK PTE LTD BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES
 * LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON
 * BRIDGETEK PARTS.
 *
 * BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE
 * INFORMATION IS NOT MODIFIED.
 *
 * IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED,
 * IT IS THE RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES
 * AND SUBSEQUENT WHQL RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.
 */

#include "displaysettings.h"

#include "platform.h"
#include <cassert>

/*****************************************************************************/

/*
 * Gpu_Hal_Init()
 */
void Gpu_Hal_Init()
{
    platform_init ();
}


/*
 * Gpu_Hal_Open()
 */
int Gpu_Hal_Open (Gpu_Hal_Context_t *host)
{
    int ret;

    pinMode(GPIO_CS, OUTPUT);
    pinMode(GPIO_PD, OUTPUT);
    pinMode(GPIO_INT, INPUT);

    platform_gpio_value (GPIO_PD, GPIO_HIGH);
    platform_gpio_value (GPIO_CS, GPIO_HIGH);

    /* init SPI */
    ret = platform_spi_init();

    /* initialize the context valriables */
    host->cmd_fifo_wp = host->dl_buff_wp = 0;
    host->spinumdummy = GPU_SPI_ONEDUMMY;
    host->status = GPU_HAL_OPENED;

    return ret;
}


/*
 * Gpu_Hal_Close()
 */
void Gpu_Hal_Close (Gpu_Hal_Context_t *host)
{
    /* deinit SPI */
    platform_spi_deinit ();
    host->status = GPU_HAL_CLOSED;
}


/*
 * Gpu_Hal_DeInit()
 */
void Gpu_Hal_DeInit()
{
    return;
}

/*****************************************************************************/

/*
 * Gpu_Hal_StartTransfer
 */
void Gpu_Hal_StartTransfer (Gpu_Hal_Context_t  *host,
                            GPU_TRANSFERDIR_T   rw,
                            uint32_t            addr)
{
    if (GPU_READ == rw)
    {
        uchar8_t transfer[4];

        transfer[0] = (addr >> 16) & 0xFF;
        transfer[1] = (addr & 0xFF00) >> 8;
        transfer[2] = addr & 0xFF;
        transfer[3] = DUMMY_BYTE;

        platform_gpio_value (GPIO_CS, GPIO_LOW);

        platform_spi_send_data (transfer, sizeof(transfer));

        host->status = GPU_HAL_READING;
    }
    else
    {
        uchar8_t transfer[3];

        transfer[0] = ((addr >> 16) & 0xBF) | 0x80;
        transfer[1] = (addr & 0xFF00) >> 8;
        transfer[2] = addr & 0xFF;

        platform_gpio_value (GPIO_CS, GPIO_LOW);

        platform_spi_send_data (transfer, sizeof(transfer));

        host->status = GPU_HAL_WRITING;
    }
}


/*
 * Gpu_Hal_StartCmdTransfer()
 */
void Gpu_Hal_StartCmdTransfer (Gpu_Hal_Context_t  *host,
                               GPU_TRANSFERDIR_T   rw)
{
    Gpu_Hal_StartTransfer(host, rw, host->cmd_fifo_wp + RAM_CMD);
}


/*
 * Gpu_Hal_TransferString()
 */
void Gpu_Hal_TransferString (const char *string)
{
    size_t length = strlen(string);

    while(length --)
    {
        Gpu_Hal_Transfer8(*string);
        string ++;
    }

    /* append one null as ending flag */
    Gpu_Hal_Transfer8(0);
}


/*
 * Gpu_Hal_Transfer8()
 */
unsigned char Gpu_Hal_Transfer8 (unsigned char value)
{
    return platform_spi_send_recv_byte (value);
}


/*
 * Gpu_Hal_Transfer16()
 */
uint16_t Gpu_Hal_Transfer16 (Gpu_Hal_Context_t  *host, uint16_t value)
{
    uint16_t retVal = 0;

    if (host->status == GPU_HAL_WRITING)
    {
        Gpu_Hal_Transfer8(value & 0xFF);
        Gpu_Hal_Transfer8((value >> 8) & 0xFF);
    }
    else
    {
        retVal = Gpu_Hal_Transfer8(0);
        retVal |= static_cast<uint16_t>(Gpu_Hal_Transfer8(0)) << 8;
    }

    return retVal;
}


/*
 * Gpu_Hal_Transfer32()
 */
uint32_t
Gpu_Hal_Transfer32 (Gpu_Hal_Context_t  *host,
                    uint32_t            value)
{
    uint32_t retVal = 0;

    if (host->status == GPU_HAL_WRITING)
    {
        Gpu_Hal_Transfer16(host,value & 0xFFFF);
        Gpu_Hal_Transfer16(host,(value >> 16) & 0xFFFF);
    }
    else
    {
        retVal = Gpu_Hal_Transfer16(host,0);
        retVal |= static_cast<uint32_t>(Gpu_Hal_Transfer16(host,0)) << 16;
    }

    return retVal;
}


/*
 * Gpu_Hal_EndTransfer()
 */
void
Gpu_Hal_EndTransfer (Gpu_Hal_Context_t *host)
{
    /* TODO: this part should be platform independent */
#ifdef FT232H_MINGW_PLATFORM
    SPI_ToggleCS ((FT_HANDLE)host->hal_handle,FALSE);
#else
    platform_gpio_value (GPIO_CS, GPIO_HIGH);
#endif

    host->status = GPU_HAL_OPENED;
}

/*****************************************************************************/

/*
 * Gpu_Hal_Rd8()
 */
uint8_t
Gpu_Hal_Rd8 (Gpu_Hal_Context_t  *host,
             uint32_t            addr)
{
    uint8_t value = 0;

    Gpu_Hal_StartTransfer(host,GPU_READ,addr);
    value = Gpu_Hal_Transfer8 (0);
    Gpu_Hal_EndTransfer(host);

    return value;
}


/*
 * Gpu_Hal_Rd16()
 */
uint16_t
Gpu_Hal_Rd16 (Gpu_Hal_Context_t  *host,
              uint32_t            addr)
{
    uint16_t value = 0;

    Gpu_Hal_StartTransfer(host,GPU_READ,addr);
    value = Gpu_Hal_Transfer16 (host, 0);
    Gpu_Hal_EndTransfer(host);

    return value;
}


/*
 * Gpu_Hal_Rd32()
 */
uint32_t
Gpu_Hal_Rd32 (Gpu_Hal_Context_t  *host,
              uint32_t            addr)
{
    uint32_t value = 0;

    Gpu_Hal_StartTransfer(host,GPU_READ,addr);
    value = Gpu_Hal_Transfer32 (host, 0);
    Gpu_Hal_EndTransfer(host);

    return value;
}

/*****************************************************************************/

/*
 * Gpu_Hal_Wr8()
 */
void
Gpu_Hal_Wr8 (Gpu_Hal_Context_t  *host,
             uint32_t            addr,
             uint8_t             v)
{
    Gpu_Hal_StartTransfer(host, GPU_WRITE, addr);
    Gpu_Hal_Transfer8(v);
    Gpu_Hal_EndTransfer(host);
}


/*
 * Gpu_Hal_Wr16()
 */
void
Gpu_Hal_Wr16 (Gpu_Hal_Context_t  *host,
              uint32_t            addr,
              uint16_t            v)
{
    Gpu_Hal_StartTransfer(host,GPU_WRITE,addr);
    Gpu_Hal_Transfer16(host,v);
    Gpu_Hal_EndTransfer(host);
}


/*
 * Gpu_Hal_Wr32()
 */
void
Gpu_Hal_Wr32 (Gpu_Hal_Context_t  *host,
              uint32_t            addr,
              uint32_t            v)
{
    Gpu_Hal_StartTransfer(host,GPU_WRITE,addr);
    Gpu_Hal_Transfer32(host,v);
    Gpu_Hal_EndTransfer(host);
}

/*****************************************************************************/

/*
 * Gpu_HostCommand()
 */
void Gpu_HostCommand (uint8_t cmd)
{
    uchar8_t transfer[3];

    transfer[0] = cmd;
    transfer[1] = 0x00;
    transfer[2] = 0x00;

    platform_gpio_value (GPIO_CS, GPIO_LOW);

    platform_spi_send_data (transfer, sizeof(transfer));

    platform_gpio_value (GPIO_CS, GPIO_HIGH);
}


/*
 * Gpu_HostCommand_Ext3()
 */
void Gpu_HostCommand_Ext3 (uint32_t cmd)
{
    uchar8_t transfer[3];

    transfer[0] = cmd & 0xFF;
    transfer[1] = (cmd >> 8) & 0xff;
    transfer[2] = (cmd >> 16) & 0xff;

    platform_gpio_value (GPIO_CS, GPIO_LOW);

    platform_spi_send_data (transfer, sizeof(transfer));

    platform_gpio_value (GPIO_CS, GPIO_HIGH);
}

/*****************************************************************************/

/*
 * Gpu_Hal_Powercycle()
 */
void Gpu_Hal_Powercycle (bool up)
{
    if (up)
    {
        platform_gpio_value (GPIO_PD, GPIO_LOW);
        Gpu_Hal_Sleep(20);
        platform_gpio_value (GPIO_PD, GPIO_HIGH);
        Gpu_Hal_Sleep(20);
    }
    else
    {
        platform_gpio_value (GPIO_PD, GPIO_HIGH);
        Gpu_Hal_Sleep(20);
        platform_gpio_value (GPIO_PD, GPIO_LOW);
        Gpu_Hal_Sleep(20);
    }
}


/*
 * Gpu_Hal_Sleep()
 */
void
Gpu_Hal_Sleep (uint32_t ms)
{
    platform_sleep_ms (ms);
}

/*****************************************************************************/

/*
 * Gpu_Hal_WrMem()
 */
void
Gpu_Hal_WrMem (Gpu_Hal_Context_t  *host,
               uint32_t            addr,
               const uint8_t      *buffer,
               uint32_t            length)
{
    Gpu_Hal_StartTransfer(host,GPU_WRITE,addr);

    /* buffer += platform_spi_send_data (host, (uchar8_t*)buffer, length, 0); */
    while (length--)
    {
        Gpu_Hal_Transfer8(*buffer);
        buffer++;
    }

    Gpu_Hal_EndTransfer(host);
}


/*
 * Gpu_Hal_RdMem()
 */
void
Gpu_Hal_RdMem (Gpu_Hal_Context_t  *host,
               uint32_t            addr,
               uint8_t            *buffer,
               uint32_t            length)
{
    Gpu_Hal_StartTransfer(host,GPU_READ,addr);

    /* platform_spi_recv_data (host, (uchar8_t*) buffer, length, 0); */
    while (length--)
    {
        *buffer = Gpu_Hal_Transfer8(0);
        buffer++;
    }

    Gpu_Hal_EndTransfer(host);
}


/*
 * Gpu_Hal_DLSwap()
 *
 * API to check the status of previous DLSWAP and perform DLSWAP of new DL.
 * Check for the status of previous DLSWAP and if still not done wait for
 * few ms and check again
 */
void
Gpu_Hal_DLSwap (Gpu_Hal_Context_t  *host,
                uint8_t             DL_Swap_Type)
{
    uint8_t Swap_Type = DLSWAP_FRAME,Swap_Done = DLSWAP_FRAME;

    if (DL_Swap_Type == DLSWAP_LINE)
        Swap_Type = DLSWAP_LINE;

    /* perform a new DL swap */

    Gpu_Hal_Wr8(host, REG_DLSWAP, Swap_Type);

    /* wait till the swap is done */
    while(Swap_Done)
    {
        Swap_Done = Gpu_Hal_Rd8(host, REG_DLSWAP);
        if(DLSWAP_DONE != Swap_Done)
            Gpu_Hal_Sleep(1);
    }
}


/*
 * Gpu_Hal_ResetDLBuffer()
 */
void
Gpu_Hal_ResetDLBuffer(Gpu_Hal_Context_t *host)
{
    host->dl_buff_wp = 0;
}

/*****************************************************************************/

void Gpu_ClockSelect (GPU_PLL_SOURCE_T pllsource)
{
    Gpu_HostCommand(pllsource);
}

void Gpu_PLL_FreqSelect (GPU_PLL_FREQ_T freq)
{
    Gpu_HostCommand(freq);
}

void Gpu_PowerModeSwitch (GPU_POWER_MODE_T pwrmode)
{
    Gpu_HostCommand(pwrmode);
}

void Gpu_CoreReset ()
{
    Gpu_HostCommand(0x68);
}

/*****************************************************************************/

/*
 * Gpu_81X_SelectSysCLK()
 *
 * This API can only be called when PLL is stopped (SLEEP mode).
 * For compatibility, set frequency to the GPU_12MHZ option in the
 * GPU_SETPLLSP1_T table.
 */
void Gpu_81X_SelectSysCLK (GPU_81X_PLL_FREQ_T  freq)
{
    assert(!EVE1 && "this function is worked only on EVE2 or EVE3");
    if(GPU_SYSCLK_72M == freq)
        Gpu_HostCommand_Ext3(0x4661);
    else if(GPU_SYSCLK_60M == freq)
        Gpu_HostCommand_Ext3(0x4561);
    else if(GPU_SYSCLK_48M == freq)
        Gpu_HostCommand_Ext3(0x4461);
    else if(GPU_SYSCLK_36M == freq)
        Gpu_HostCommand_Ext3(0x361);
    else if(GPU_SYSCLK_24M == freq)
        Gpu_HostCommand_Ext3(0x261);
    else if(GPU_SYSCLK_DEFAULT == freq)
        Gpu_HostCommand_Ext3(0x61);
}


/*
 * Gpu_81X_PowerOffComponents()
 *
 * Power down or up ROMs and ADCs. Specified one or more elements in the
 * GPU_81X_ROM_AND_ADC_T table to power down, unspecified elements will be
 * powered up. The application must retain the state of the ROMs and ADCs
 * as they're not readable from the device.
 */
void
Gpu_81X_PowerOffComponents (Gpu_Hal_Context_t  *host,
                            uint8_t             val)
{
    assert(!EVE1 && "this function is worked only on EVE2 or EVE3");
    Gpu_HostCommand_Ext3((uint32_t)0x49 | (val<<8));
}


/*
 * Gpu_81X_PadDriveStrength()
 *
 * This API sets the current strength of supported GPIO/IO group(s).
 */
void
Gpu_81X_PadDriveStrength (Gpu_Hal_Context_t              *host,
                          GPU_81X_GPIO_DRIVE_STRENGTH_T   strength,
                          GPU_81X_GPIO_GROUP_T            group)
{
    assert(!EVE1 && "this function is worked only on EVE2 or EVE3");
    Gpu_HostCommand_Ext3((uint32_t)0x70 | (group << 8) | (strength << 8));
}


/*
 * Gpu_81X_ResetActive()
 *
 * This API will hold the system reset active, Gpu_81X_ResetRemoval() must be
 * called to release the system reset.
 */
void Gpu_81X_ResetActive ()
{
    assert(!EVE1 && "this function is worked only on EVE2 or EVE3");
    Gpu_HostCommand_Ext3(GPU_81X_RESET_ACTIVE);
}


/*
 * Gpu_81X_ResetRemoval()
 *
 * This API will release the system reset, and the system will exit reset and
 * behave as after POR, settings done through SPI will not be affected.
 */
void Gpu_81X_ResetRemoval ()
{
    assert(!EVE1 && "this function is worked only on EVE2 or EVE3");
    Gpu_HostCommand_Ext3(GPU_81X_RESET_REMOVAL);
}


/*
 * Gpu_Hal_SetSPI()
 *
 * Set EVE spi communication mode
 */
int16_t Gpu_Hal_SetSPI (Gpu_Hal_Context_t      *host,
                GPU_SPI_NUMCHANNELS_T   numchnls,
                GPU_SPI_NUMDUMMYBYTES   numdummy)
{
    if (EVE1) return 0;

    uint8_t writebyte = 0;

    if((numchnls > GPU_SPI_QUAD_CHANNEL) ||
            (numdummy > GPU_SPI_TWODUMMY) ||
            (numdummy < GPU_SPI_ONEDUMMY))
        return -1;

    /* swicth EVE to multi channel SPI mode */
    writebyte = numchnls;
    if(numdummy == GPU_SPI_TWODUMMY)
        writebyte |= SPI_TWO_DUMMY_BYTE;
    Gpu_Hal_Wr8(host, REG_SPI_WIDTH, writebyte);

    /* FT81x swicthed to dual/quad mode, now update global HAL context */
    host->spichannel = numchnls;
    host->spinumdummy = numdummy;

    return 0;
}


/*****************************************************************************/

/*
 * Gpu_Hal_Updatecmdfifo()
 *
 * Function to update global HAL context variable cmd_fifo_wp pointer and write
 * to REG_CMD_WRITE to indicate GPU to start processing new commands in RAM_CMD
 */
void
Gpu_Hal_Updatecmdfifo (Gpu_Hal_Context_t  *host,
                       uint32_t            count)
{
    host->cmd_fifo_wp  = (host->cmd_fifo_wp + count) & FIFO_SIZE_MASK;

    /* 4 byte alignment */
    host->cmd_fifo_wp = (host->cmd_fifo_wp + 3) & FIFO_BYTE_ALIGNMENT_MASK;
    Gpu_Hal_Wr16(host, REG_CMD_WRITE, host->cmd_fifo_wp);
}


/*
 * Gpu_Cmdfifo_Freespace()
 *
 * Function to compute available freespace in RAM_CMD. RAM_CMD is 4K in size.
 * REG_CMD_READ reg provides command buffer read pointer.
 */
uint16_t
Gpu_Cmdfifo_Freespace (Gpu_Hal_Context_t *host)
{
    uint16_t fullness,retval;

    fullness = (host->cmd_fifo_wp-Gpu_Hal_Rd16(host, REG_CMD_READ)) & FIFO_SIZE_MASK;
    retval = (CMD_FIFO_SIZE - 4) - fullness;

    return (retval);
}


/*
 * Gpu_Hal_WrCmdBuf()
 *
 * Continuous write to RAM_CMD with wait with start address as
 * host->cmd_fifo_wp + RAM_CMD. FT81x RAM_CMD size is 4K (4096 bytes).
 * Hence one SPI write is adequate.
 */
void Gpu_Hal_WrCmdBuf (Gpu_Hal_Context_t  *host,
                  uint8_t            *buffer,
                  uint32_t            count)
{
    uint32_t length = 0, availablefreesize;

    do
    {
        length = count;
        availablefreesize = Gpu_Cmdfifo_Freespace(host);

        if (length > availablefreesize)
            length = availablefreesize;

        Gpu_Hal_CheckCmdBuffer(host, length);
        Gpu_Hal_StartCmdTransfer(host, GPU_WRITE);

        platform_spi_send_data (static_cast<uchar8_t*>(buffer), static_cast<uint16_t>(length));

        buffer += length;

        Gpu_Hal_EndTransfer(host);
        Gpu_Hal_Updatecmdfifo(host,length);
        Gpu_Hal_WaitCmdfifo_empty(host);

        count -= length;

    } while (count > 0);
}


/*
 * Gpu_Hal_CheckCmdBuffer()
 *
 * Blocking function call. Blocks until "count" number of bytes gets available
 * in RAM_CMD.
 */
void
Gpu_Hal_CheckCmdBuffer (Gpu_Hal_Context_t  *host,
                        uint32_t            count)
{
    uint16_t getfreespace;
    do
    {
        getfreespace = Gpu_Cmdfifo_Freespace(host);
    } while(getfreespace < count);
}


/*
 * Gpu_Hal_WaitCmdfifo_empty()
 *
 * Blocking function call. Blocks until all commands in RAM_CMD are executed and
 * it is fully empty.
 */
void
Gpu_Hal_WaitCmdfifo_empty (Gpu_Hal_Context_t *host)
{
    while(Gpu_Hal_Rd16(host, REG_CMD_READ) != Gpu_Hal_Rd16(host, REG_CMD_WRITE));
    host->cmd_fifo_wp = Gpu_Hal_Rd16(host, REG_CMD_WRITE);
}


/*
 * Gpu_Hal_WrCmdBuf_nowait()
 *
 * Continuous write to RAM_CMD with no wait.
 */
void
Gpu_Hal_WrCmdBuf_nowait (Gpu_Hal_Context_t  *host,
                         uint8_t            *buffer,
                         uint32_t            count)
{
    uint32_t length =0, availablefreesize;

    do
    {
        length = count;
        availablefreesize = Gpu_Cmdfifo_Freespace(host);

        if (length > availablefreesize)
            length = availablefreesize;

        Gpu_Hal_CheckCmdBuffer(host,length);
        Gpu_Hal_StartCmdTransfer(host, GPU_WRITE);

        platform_spi_send_data (static_cast<uchar8_t*>(buffer), static_cast<uint16_t>(length));

        buffer += length;

        Gpu_Hal_EndTransfer(host);
        Gpu_Hal_Updatecmdfifo(host,length);

        count -= length;

    } while (count > 0);
}


/*
 * Gpu_Hal_WaitCmdfifo_empty_status()
 */
uint8_t
Gpu_Hal_WaitCmdfifo_empty_status (Gpu_Hal_Context_t *host)
{
    DisplaySettings& settings = DisplaySettings::getInstance();
    if(Gpu_Hal_Rd16(host, REG_CMD_READ) != Gpu_Hal_Rd16(host, REG_CMD_WRITE))
    {
        return 0;
    }
    else
    {
        host->cmd_fifo_wp = Gpu_Hal_Rd16(host, REG_CMD_WRITE);
        return 1;
    }
}


/*
 * Gpu_Hal_WaitLogo_Finish()
 */
void
Gpu_Hal_WaitLogo_Finish (Gpu_Hal_Context_t *host)
{
    uint16_t cmdrdptr,cmdwrptr;

    DisplaySettings& settings = DisplaySettings::getInstance();
    do
    {
        cmdrdptr = Gpu_Hal_Rd16(host, REG_CMD_READ);
        cmdwrptr = Gpu_Hal_Rd16(host, REG_CMD_WRITE);
    } while ((cmdwrptr != cmdrdptr) || (cmdrdptr != 0));

    host->cmd_fifo_wp = 0;
}


/*
 * Gpu_Hal_ResetCmdFifo()
 */
void
Gpu_Hal_ResetCmdFifo (Gpu_Hal_Context_t *host)
{
    host->cmd_fifo_wp = 0;
}


/*
 * Gpu_Hal_WrCmd32()
 */
void
Gpu_Hal_WrCmd32 (Gpu_Hal_Context_t  *host,
                 uint32_t            cmd)
{
    Gpu_Hal_CheckCmdBuffer(host,sizeof(cmd));
    Gpu_Hal_Wr32(host,RAM_CMD + host->cmd_fifo_wp,cmd);
    Gpu_Hal_Updatecmdfifo(host,sizeof(cmd));
}

/*****************************************************************************/

/*
 * Fifo_Init()
 *
 * Init all the parameters of fifo buffer.
 */
void
Fifo_Init (Fifo_t    *pFifo,
           uint32_t   StartAddress,
           uint32_t   Length,
           uint32_t   HWReadRegAddress,
           uint32_t   HWWriteRegAddress)
{
    /* update the context parameters */
    pFifo->fifo_buff = StartAddress;
    pFifo->fifo_len = Length;
    pFifo->fifo_rp = pFifo->fifo_wp = 0;

    /* update the hardware register addresses - specific to FT800 series chips */
    pFifo->HW_Read_Reg = HWReadRegAddress;
    pFifo->HW_Write_Reg = HWWriteRegAddress;
}


/*
 * Fifo_Update()
 *
 * Update both the read and write pointers.
 */
void
Fifo_Update (Gpu_Hal_Context_t  *host,
             Fifo_t             *pFifo)
{
    pFifo->fifo_rp = Gpu_Hal_Rd32(host,pFifo->HW_Read_Reg);
}


/*
 * Fifo_Write()
 *
 * Just write and update the write register.
 */
uint32_t
Fifo_Write (Gpu_Hal_Context_t  *host,
            Fifo_t             *pFifo,
            uint8_t            *buffer,
            uint32_t            NumbytetoWrite)
{
    uint32_t FreeSpace = Fifo_GetFreeSpace(host,pFifo),TotalBytes = NumbytetoWrite;

    if(NumbytetoWrite > FreeSpace)
    {
        /* update the read pointer and get the free space */
        Fifo_Update(host,pFifo);
        FreeSpace = Fifo_GetFreeSpace(host,pFifo);

        if(NumbytetoWrite > FreeSpace)
            TotalBytes = FreeSpace;
    }

    /* sanity check */
    if(TotalBytes <= 0)
        return 0; /* error condition */

    /* check for the loopback conditions */
    if(pFifo->fifo_wp + TotalBytes >= pFifo->fifo_len)
    {
        uint32_t partialchunk, secpartialchunk;
        partialchunk = pFifo->fifo_len - pFifo->fifo_wp;
        secpartialchunk = TotalBytes - partialchunk;

        Gpu_Hal_WrMem(host,pFifo->fifo_buff + pFifo->fifo_wp,buffer,partialchunk);
        if(secpartialchunk > 0)
            Gpu_Hal_WrMem(host,pFifo->fifo_buff,buffer + partialchunk,secpartialchunk);

        pFifo->fifo_wp = secpartialchunk;
    }
    else
    {
        Gpu_Hal_WrMem(host,pFifo->fifo_buff + pFifo->fifo_wp,buffer,TotalBytes);
        pFifo->fifo_wp += TotalBytes;
    }

    /* update the write pointer address in write register */
    Gpu_Hal_Wr32(host,pFifo->HW_Write_Reg,pFifo->fifo_wp);

    return TotalBytes;
}


/*
 * Fifo_Write32()
 *
 * Just write one word and update the write register
 */
void
Fifo_Write32 (Gpu_Hal_Context_t  *host,
              Fifo_t             *pFifo,
              uint32_t            WriteWord)
{
    Fifo_WriteWait(host,pFifo,(uint8_t *)&WriteWord,4);
}


/*
 * Fifo_WriteWait()
 *
 * Write and wait for the fifo to be empty. Handle cases even if the Numbytes
 * are more than freespace.
 */
void
Fifo_WriteWait (Gpu_Hal_Context_t  *host,
                Fifo_t             *pFifo,
                uint8_t            *buffer,
                uint32_t            Numbyte)
{
    uint32_t TotalBytes = Numbyte,currchunk = 0,FreeSpace;
    uint8_t *pbuff = buffer;

    /* blocking call, manage to check for the error case and break in case of error */
    while(TotalBytes > 0)
    {
        currchunk = TotalBytes;
        FreeSpace = Fifo_GetFreeSpace(host,pFifo);

        if(currchunk > FreeSpace)
            currchunk = FreeSpace;

        Fifo_Write(host,pFifo,pbuff,currchunk);
        pbuff += currchunk;
        TotalBytes -= currchunk;
    }
}


/*
 * Fifo_GetFreeSpace()
 *
 * Get the free space in the fifo - make sure the return value is maximum
 * of (LENGTH - 4).
 */
uint32_t
Fifo_GetFreeSpace (Gpu_Hal_Context_t  *host,
                   Fifo_t             *pFifo)
{
    uint32_t FreeSpace = 0;

    Fifo_Update(host,pFifo);

    if(pFifo->fifo_wp >= pFifo->fifo_rp)
        FreeSpace = pFifo->fifo_len - pFifo->fifo_wp + pFifo->fifo_rp;
    else
        FreeSpace = pFifo->fifo_rp - pFifo->fifo_wp;

    if(FreeSpace >= 4)
        FreeSpace -= 4;

    return FreeSpace;
}

/*****************************************************************************/

/*
 * Gpu_Hal_Dec2Ascii()
 */
int32_t
Gpu_Hal_Dec2Ascii (char *pSrc,
                   int32_t   value)
{
    int16_t Length;
    char8_t *pdst,charval;
    int32_t CurrVal = value,tmpval,i;
    char8_t tmparray[16];
    uchar8_t idx = 0;

    Length = strlen(pSrc);
    pdst = pSrc + Length;

    if(0 == value)
    {
        *pdst++ = '0';
        *pdst++ = '\0';
        return 0;
    }

    if(CurrVal < 0)
    {
        *pdst++ = '-';
        CurrVal = - CurrVal;
    }
    /* insert the value */
    while(CurrVal > 0){
        tmpval = CurrVal;
        CurrVal /= 10;
        tmpval = tmpval - CurrVal*10;
        charval = '0' + tmpval;
        tmparray[idx++] = charval;
    }

    for(i=0;i<idx;i++)
    {
        *pdst++ = tmparray[idx - i - 1];
    }
    *pdst++ = '\0';

    return 0;
}


const uint8_t DLCODE_BOOTUP[12] =
{
    0,0,0,2,	//GPU instruction CLEAR_COLOR_RGB
    7,0,0,38,	//GPU instruction CLEAR
    0,0,0,0,	//GPU instruction DISPLAY
};


/*
 * Gpu_ClearScreen()
 */
void
Gpu_ClearScreen (Gpu_Hal_Context_t *host)
{
    DisplaySettings& settings = DisplaySettings::getInstance();

    Gpu_Hal_WrMem(host, RAM_DL,const_cast<uint8_t*>(DLCODE_BOOTUP),sizeof(DLCODE_BOOTUP));
    Gpu_Hal_Wr8(host, REG_DLSWAP, DLSWAP_FRAME);
}
