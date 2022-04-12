/*
 * Copyright (c) Riverdi Sp. z o.o. sp. k. <riverdi@riverdi.com>
 * Copyright (c) Skalski Embedded Technologies <contact@lukasz-skalski.com>
 */

#include "platform.h"

/*
 * platform_init()
 */
int platform_init ()
{
    /* intentionally empty */
    return wiringPiSetup();
}


/*
 * platform_sleep_ms()
 */
void
platform_sleep_ms (uint32_t ms)
{
    usleep (ms*1000);
}


/*
 * platform_spi_init()
 */
int platform_spi_init ()
{
    return wiringPiSPISetupMode(0, 5000000, 0);
}


/*
 * platform_spi_deinit()
 */
void platform_spi_deinit ()
{
}


/*
 * platform_spi_send_recv_byte();
 */
uchar8_t platform_spi_send_recv_byte (uchar8_t data)
{
    unsigned char tmp_data = data;
    wiringPiSPIDataRW(0, &tmp_data, 1);
    return tmp_data;
}


/*
 * platform_spi_send_data()
 */
void platform_spi_send_data (uchar8_t *data, uint16_t size)
{
    wiringPiSPIDataRW(0, data, size);
}


/*
 * platform_spi_recv_data()
 */
void platform_spi_recv_data (uchar8_t *data, uint16_t size)
{
    wiringPiSPIDataRW(0, data, size);
}

/*
 * platform_gpio_value()
 */
void platform_gpio_value (gpio_name ngpio, gpio_val vgpio)
{
    digitalWrite(ngpio, vgpio);
}


