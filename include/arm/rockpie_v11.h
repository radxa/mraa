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

#define MRAA_ROCKPIE_V11_GPIO_COUNT 27
#define MRAA_ROCKPIE_V11_I2C_COUNT  1
#define MRAA_ROCKPIE_V11_SPI_COUNT  1
#define MRAA_ROCKPIE_V11_UART_COUNT 2
#define MRAA_ROCKPIE_V11_PWM_COUNT  0
#define MRAA_ROCKPIE_V11_AIO_COUNT  1
#define MRAA_ROCKPIE_V11_PIN_COUNT  40

mraa_board_t *
        mraa_rockpie_v11();

#ifdef __cplusplus
}
#endif
