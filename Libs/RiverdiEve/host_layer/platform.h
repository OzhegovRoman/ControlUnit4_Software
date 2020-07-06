/*
 * Copyright (c) Riverdi Sp. z o.o. sp. k. <riverdi@riverdi.com>
 * Copyright (c) Skalski Embedded Technologies <contact@lukasz-skalski.com>
 */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/* C library inclusions */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include </home/roman/raspi/sysroot/usr/local/include/wiringPiSPI.h>
#include </home/roman/raspi/sysroot/usr/local/include/wiringPi.h>

/*****************************************************************************/

/* type definitions for EVE HAL library */

#define TRUE		(1)
#define FALSE		(0)

typedef char		bool_t;
typedef char		char8_t;
typedef unsigned char	uchar8_t;
typedef signed char	schar8_t;
typedef float		float_t;

/* EVE inclusions */
#include "../eve_layer/Gpu_Hal.h"
#include "../eve_layer/Gpu.h"
#include "../eve_layer/CoPro_Cmds.h"
#include "../eve_layer/Hal_Utils.h"

/*****************************************************************************/

#define SPI_DEVICE	"/dev/spidev0.0"
#define SPI_MODE	0
#define SPI_BITS	8
#define SPI_SPEED_HZ	30000000

//typedef enum {
//  GPIO_CS   = 5,
//  GPIO_PD   = 19,
//  GPIO_INT  = 6
//} gpio_name;
typedef enum {
  GPIO_CS   = 21,
  GPIO_PD   = 24,
  GPIO_INT  = 22
} gpio_name;

typedef enum {
  GPIO_HIGH = 1,
  GPIO_LOW  = 0
} gpio_val;

/*****************************************************************************/

int platform_init();
void platform_sleep_ms (uint32_t);

int platform_spi_init();
void platform_spi_deinit ();

uchar8_t platform_spi_send_recv_byte (uchar8_t data);
void platform_spi_send_data(uchar8_t *data, uint16_t size);
void platform_spi_recv_data (uchar8_t *data, uint16_t size);

void platform_gpio_value (gpio_name, gpio_val);

#endif /*_PLATFORM_H_*/
