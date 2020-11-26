/*
 * Author: Shine <yll@radxa.com>
 * Copyright (c) 2020 Vamrs Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

// +1 as pins are "1 indexed"
#define MRAA_ROCKPIX_PINCOUNT (40 + 1)
#define MRAA_ROCKPIX_GPIOCOUNT (10)
#define MRAA_ROCKPIX_PWM_COUNT 2
#define MRAA_ROCKPIX_SPI_COUNT 1
#define MRAA_ROCKPIX_UART_COUNT 1
#define MRAA_ROCKPIX_ADC_COUNT 0

mraa_board_t*
mraa_rockpix_board();

#ifdef __cplusplus
}
#endif
