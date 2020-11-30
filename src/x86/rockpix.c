/*
 * Author: Shine <yll@radxa.com>
 * Copyright (c) 2019 Vamrs Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "x86/rockpix.h"

#define PLATFORM_NAME "ROCK PI X"
#define PLATFORM_VERSION "1.0.0"

#define MRAA_ROCK_PI_X_VIRTUAL   220  /* 0~7 */
#define MRAA_ROCK_PI_X_SOUTHEAST 228  /* 0~85 */
#define MRAA_ROCK_PI_X_EAST      314  /* 0~26 */
#define MRAA_ROCK_PI_X_NORTH     341  /* 0~72 */
#define MRAA_ROCK_PI_X_SOUTHWEST 414  /* 0~97 */

static mraa_result_t
mraa_rockpix_set_pininfo(mraa_board_t* board, int mraa_index, char* name, mraa_pincapabilities_t caps, int sysfs_pin)
{
    if (mraa_index < board->phy_pin_count) {
        mraa_pininfo_t* pin_info = &board->pins[mraa_index];
        strncpy(pin_info->name, name, MRAA_PIN_NAME_SIZE);
        pin_info->capabilities = caps;
        if (caps.gpio) {
            pin_info->gpio.pinmap = sysfs_pin;
            pin_info->gpio.mux_total = 0;
        }
        if (caps.i2c) {
            pin_info->i2c.pinmap = 1;
            pin_info->i2c.mux_total = 0;
        }
        if (caps.pwm) {
            pin_info->pwm.parent_id = 0;
            pin_info->pwm.pinmap = 0;
            pin_info->pwm.mux_total = 0;
        }
        if (caps.spi) {
            pin_info->spi.mux_total = 0;
        }
        if (caps.uart) {
            pin_info->uart.mux_total = 0;
        }
        return MRAA_SUCCESS;
    }
    return MRAA_ERROR_INVALID_RESOURCE;
}

static mraa_result_t
mraa_rockpix_get_pin_index(mraa_board_t* board, char* name, int* pin_index)
{
    int i;
    for (i = 0; i < board->phy_pin_count; ++i) {
        if (strncmp(name, board->pins[i].name, MRAA_PIN_NAME_SIZE) == 0) {
            *pin_index = i;
            return MRAA_SUCCESS;
        }
    }

    syslog(LOG_CRIT, "ROCK PI X: Failed to find pin name %s", name);

    return MRAA_ERROR_INVALID_RESOURCE;
}

mraa_board_t*
mraa_rockpix_board()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));

    if (b == NULL) {
        return NULL;
    }

    b->platform_name = PLATFORM_NAME;
    b->platform_version = PLATFORM_VERSION;
    b->phy_pin_count = MRAA_ROCKPIX_PINCOUNT;
    b->gpio_count = MRAA_ROCKPIX_GPIOCOUNT;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * MRAA_ROCKPIX_PINCOUNT);
    if (b->pins == NULL) {
        goto error;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        goto error;
    }

    mraa_rockpix_set_pininfo(b, 0,  "INVALID",     (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 1,  "3.3v",        (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 2,  "5v",          (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 3,  "I2C2_SDA",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 4,  "5v",          (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 5,  "I2C2_SCL",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 6,  "GND",         (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 7,  "ISH_GPIO0",   (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 }, 335);
    mraa_rockpix_set_pininfo(b, 8,  "UART2_TX",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
    mraa_rockpix_set_pininfo(b, 9,  "GND",         (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 10, "UART2_RX",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
    mraa_rockpix_set_pininfo(b, 11, "SPI2_CLK",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 1 }, -1);
    mraa_rockpix_set_pininfo(b, 12, "GPIO_DFX3",   (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 345);
    mraa_rockpix_set_pininfo(b, 13, "SPI2_MISO",   (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 1 }, -1);
    mraa_rockpix_set_pininfo(b, 14, "GND",         (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 15, "SPI2_CS0",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 16, "SPI2_MOSI",   (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 }, 346);
    mraa_rockpix_set_pininfo(b, 17, "+3.3V",       (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 18, "UART2_RTS",   (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
    mraa_rockpix_set_pininfo(b, 19, "SPI_MOSI",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 1, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 20, "GND",         (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 21, "SPI_MISO",    (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 }, 334);
    mraa_rockpix_set_pininfo(b, 22, "UART2_CTS",   (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
    mraa_rockpix_set_pininfo(b, 23, "SPI_CLK",     (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 1, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 24, "ISH_GPIO1",   (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 332);
    mraa_rockpix_set_pininfo(b, 25, "GND",         (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 26, "ISH_GPIO4",   (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 336);
    mraa_rockpix_set_pininfo(b, 27, "I2C5_DATA",   (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 28, "I2C5_CLK",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 29, "ISH_GPIO2",   (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 338);
    mraa_rockpix_set_pininfo(b, 30, "GND",         (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 31, "ISH_GPIO3",   (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 329);
    mraa_rockpix_set_pininfo(b, 32, "PWM0",        (mraa_pincapabilities_t){ 1, 0, 1, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 33, "PWM1",        (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 1 }, 229);
    mraa_rockpix_set_pininfo(b, 34, "GND",         (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 35, "I2S1_FRM",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 36, "I2S1_CLK",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 37, "ISH_GPIO13",  (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 348);
    mraa_rockpix_set_pininfo(b, 38, "I2S1_RX",     (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 39, "GND",         (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_rockpix_set_pininfo(b, 40, "I2S1_TX",     (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);

    // Set number of i2c adaptors usable from userspace
    b->i2c_bus_count = 0;
    b->def_i2c_bus = 0;
    int i2c_bus_num;

    // Configure i2c adaptor #0 (default)
    i2c_bus_num = mraa_find_i2c_bus_pci("0000:00", "808622C1:02", ".");
    if (i2c_bus_num != -1) {
        int i = b->i2c_bus_count;
        b->i2c_bus[i].bus_id = i2c_bus_num;
        mraa_rockpix_get_pin_index(b, "I2C2_SDA", (int*) &(b->i2c_bus[0].sda));
        mraa_rockpix_get_pin_index(b, "I2C2_SCL", (int*) &(b->i2c_bus[0].scl));
        b->i2c_bus_count++;
    }

    // Configure i2c adaptor #1
    i2c_bus_num = mraa_find_i2c_bus_pci("0000:00", "808622C1:05", ".");
    if (i2c_bus_num != -1) {
        int i = b->i2c_bus_count;
        b->i2c_bus[i].bus_id = i2c_bus_num;
        mraa_rockpix_get_pin_index(b, "I2C5_DATA", (int*) &(b->i2c_bus[1].sda));
        mraa_rockpix_get_pin_index(b, "I2C5_CLK", (int*) &(b->i2c_bus[1].scl));
        b->i2c_bus_count++;
    }

    // Configure PWM
    b->pwm_dev_count = 0;
    b->def_pwm_dev = 0;
    b->pwm_default_period = 5000;
    b->pwm_max_period = 218453;
    b->pwm_min_period = 1;

    // set the correct pwm channels for pwm 1 2
    b->pins[32].pwm.parent_id = 0;
    b->pins[32].pwm.pinmap = 0;
    b->pwm_dev_count++;

    // Configure SPI #0 CS0 (default)
    b->spi_bus_count = MRAA_ROCKPIX_SPI_COUNT;
    b->spi_bus[0].bus_id = 2;
    b->spi_bus[0].slave_s = 0;
    mraa_rockpix_get_pin_index(b, "SPI2_CS0", (int*) &(b->spi_bus[0].cs));
    mraa_rockpix_get_pin_index(b, "SPI2_MOSI", (int*) &(b->spi_bus[0].mosi));
    mraa_rockpix_get_pin_index(b, "SPI2_MISO", (int*) &(b->spi_bus[0].miso));
    mraa_rockpix_get_pin_index(b, "SPI2_CLK", (int*) &(b->spi_bus[0].sclk));
    b->def_spi_bus = 0;

    // Configure UART #1 (default)
    b->uart_dev_count = MRAA_ROCKPIX_UART_COUNT;
    mraa_rockpix_get_pin_index(b, "UART2_RX",  &(b->uart_dev[0].rx));
    mraa_rockpix_get_pin_index(b, "UART2_TX",  &(b->uart_dev[0].tx));
    mraa_rockpix_get_pin_index(b, "UART2_CTS", &(b->uart_dev[0].cts));
    mraa_rockpix_get_pin_index(b, "UART2_RTS", &(b->uart_dev[0].rts));
    b->uart_dev[0].device_path = "/dev/ttyS5";
    b->def_uart_dev = 0;

    // Configure ADCs
    b->aio_count = MRAA_ROCKPIX_ADC_COUNT;

    const char* pinctrl_path = "/sys/bus/platform/drivers/cherryview-pinctrl";
    int have_pinctrl = access(pinctrl_path, F_OK) != -1;
    syslog(LOG_NOTICE, "ROCK PI X: kernel pinctrl driver %savailable", have_pinctrl ? "" : "un");

    if (have_pinctrl)
        return b;

error:
    syslog(LOG_CRIT, "ROCK PI X: Platform failed to initialise");
    free(b);
    return NULL;
}
