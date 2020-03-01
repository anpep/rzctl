/*
 * rzctl(1) -- Razer mouse control utility
 * Copyright (c) 2019 Angel <angel@ttm.sh>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdint.h>
#include "razer.h"

/* obtains the current brightness value of the device */
int razer_get_brightness(struct razer_device *dev);

/* sets the brightness value for the device (0-100) */
int razer_set_brightness(struct razer_device *dev, uint8_t brightness);

/* set-brightness CLI command */
int set_brightness(int verbose, int brightness, int fade);
