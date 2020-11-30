/*
 * Author: Shine <yll@radxa.com>
 * Copyright (c) Radxa Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#include <mraa/common.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "arm/rockpie.h"
#include "common.h"

#define DT_BASE "/proc/device-tree"
/*
* "Radxa ROCK Pi 4" is the model name on stock 5.x kernels
* "ROCK PI 4A", "ROCK PI 4B" is used on Radxa 4.4 kernel
* so we search for the string below by ignoring case
*/
#define PLATFORM_NAME_ROCK_PI_E "ROCK Pi E"
#define MAX_SIZE 64

const char* rockpie_serialdev[MRAA_ROCKPIE_UART_COUNT] = { "/dev/ttyS1","/dev/ttyS2"};

void
mraa_rockpie_pininfo(mraa_board_t* board, int index, int sysfs_pin, mraa_pincapabilities_t pincapabilities_t, char* fmt, ...)
{

    va_list arg_ptr;

    if (index > board->phy_pin_count)
        return;

    mraa_pininfo_t* pininfo = &board->pins[index];
    va_start(arg_ptr, fmt);
    vsnprintf(pininfo->name, MRAA_PIN_NAME_SIZE, fmt, arg_ptr);

    if( pincapabilities_t.gpio == 1 ) {
	va_arg(arg_ptr, int);
	pininfo->gpio.gpio_chip = va_arg(arg_ptr, int);
	pininfo->gpio.gpio_line = va_arg(arg_ptr, int);
    }

    pininfo->capabilities = pincapabilities_t;

    va_end(arg_ptr);
    pininfo->gpio.pinmap = sysfs_pin;
    pininfo->gpio.mux_total = 0;
}

mraa_board_t*
mraa_rockpie()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b);
        return NULL;
    }

    // pin mux for buses are setup by default by kernel so tell mraa to ignore them
    b->no_bus_mux = 1;
    b->phy_pin_count = MRAA_ROCKPIE_PIN_COUNT + 1;

    if (mraa_file_exist(DT_BASE "/model")) {
        // We are on a modern kernel, great!!!!
        if (mraa_file_contains(DT_BASE "/model", PLATFORM_NAME_ROCK_PI_E)) {
            b->platform_name = PLATFORM_NAME_ROCK_PI_E;
            b->uart_dev[0].device_path = (char*) rockpie_serialdev[0];
            b->uart_dev[1].device_path = (char*) rockpie_serialdev[1];
        }
    }

    // UART
    b->uart_dev_count = MRAA_ROCKPIE_UART_COUNT;
    b->def_uart_dev = 0;
    b->uart_dev[0].index = 1;
    b->uart_dev[1].index = 2;

    // I2C
    if (strncmp(b->platform_name, PLATFORM_NAME_ROCK_PI_E, MAX_SIZE) == 0) {
        b->i2c_bus_count = MRAA_ROCKPIE_I2C_COUNT;
        b->def_i2c_bus = 0;
        b->i2c_bus[0].bus_id = 1;
    }

    // SPI
    b->spi_bus_count = MRAA_ROCKPIE_SPI_COUNT;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 32766;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    /* ADC */
    b->aio_count = MRAA_ROCKPIE_AIO_COUNT;
    b->adc_raw = 10;
    b->adc_supported = 10;
    b->aio_dev[0].pin = 22;
    b->aio_non_seq = 1;
    b->pins[22].aio.pinmap = 1;

    /* PWM1,2 */
    b->pwm_dev_count = MRAA_ROCKPIE_PWM_COUNT;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins[33].pwm.parent_id = 2;
    b->pins[33].pwm.mux_total = 0;
    b->pins[33].pwm.pinmap = 0;

    mraa_rockpie_pininfo(b, 0,   -1, (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, "INVALID");
    mraa_rockpie_pininfo(b, 1,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_rockpie_pininfo(b, 2,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_rockpie_pininfo(b, 3,  100, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "UART1_TX");
    mraa_rockpie_pininfo(b, 4,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_rockpie_pininfo(b, 5,  102, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "UART1_RX");
    mraa_rockpie_pininfo(b, 6,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpie_pininfo(b, 7,   60, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO1_D4");
    mraa_rockpie_pininfo(b, 8,   64, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "UART2_TX");
    mraa_rockpie_pininfo(b, 9,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpie_pininfo(b, 10,  65, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "UART2_RX");
    mraa_rockpie_pininfo(b, 11,  66, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "PWM_IR");
    mraa_rockpie_pininfo(b, 12,  82, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C2");
    mraa_rockpie_pininfo(b, 13,  67, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_A3");
    mraa_rockpie_pininfo(b, 14,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpie_pininfo(b, 15,  27, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO0_D3");
    mraa_rockpie_pininfo(b, 16,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "USB20DM");
    mraa_rockpie_pininfo(b, 17,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_rockpie_pininfo(b, 18,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "USB20DP");
    mraa_rockpie_pininfo(b, 19,  97, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "SPI0_TXD");
    mraa_rockpie_pininfo(b, 20,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpie_pininfo(b, 21,  98, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "SPI0_RXD");
    mraa_rockpie_pininfo(b, 22,  76, (mraa_pincapabilities_t){1,1,0,0,0,0,1,0}, "ADC_IN1");
    mraa_rockpie_pininfo(b, 23,  96, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "SPI0_CLK");
    mraa_rockpie_pininfo(b, 24, 104, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "SPI0_CSN0");
    mraa_rockpie_pininfo(b, 25,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpie_pininfo(b, 26,  76, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_B4");
    mraa_rockpie_pininfo(b, 27,  68, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "I2C1_SDA");
    mraa_rockpie_pininfo(b, 28,  69, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "I2C1_SCL");
    mraa_rockpie_pininfo(b, 29,  84, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C4");
    mraa_rockpie_pininfo(b, 30,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpie_pininfo(b, 31,  85, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C5");
    mraa_rockpie_pininfo(b, 32,  80, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C0");
    mraa_rockpie_pininfo(b, 33,  70, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PWM2");
    mraa_rockpie_pininfo(b, 34,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpie_pininfo(b, 35,  81, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C1");
    mraa_rockpie_pininfo(b, 36,  79, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_B7");
    mraa_rockpie_pininfo(b, 37,  86, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C6");
    mraa_rockpie_pininfo(b, 38,  83, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C3");
    mraa_rockpie_pininfo(b, 39,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpie_pininfo(b, 40,  87, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C7");

    return b;
}
