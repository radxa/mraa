/*
 * Author: Shine <yll@radxa.com>
 * Copyright (c) Radxa Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MRAA_ROCKPIE_GPIO_COUNT 25
#define MRAA_ROCKPIE_I2C_COUNT  1
#define MRAA_ROCKPIE_SPI_COUNT  1
#define MRAA_ROCKPIE_UART_COUNT 2
#define MRAA_ROCKPIE_PWM_COUNT  1
#define MRAA_ROCKPIE_AIO_COUNT  1
#define MRAA_ROCKPIE_PIN_COUNT  40

mraa_board_t *
        mraa_rockpie();

#ifdef __cplusplus
}
#endif
