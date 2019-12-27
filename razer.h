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
#include <libusb-1.0/libusb.h>

/* HID definitions */
#define HID_GET_REPORT          0x01
#define HID_SET_REPORT          0x09
#define HID_REPORT_TYPE_INPUT   0x01
#define HID_REPORT_TYPE_FEATURE 0x03

#define REPORT_TIMEOUT 1500  /* timeout for HID reports sent to devices */

/* LED definitions */
enum {
    LED_SCROLL = 0x01,
    LED_LOGO = 0x04
};

struct razer_device {
    struct libusb_device_descriptor desc;
    struct libusb_config_descriptor *cfg_desc;
    libusb_device *dev;
    libusb_device_handle *handle;

    char s_manufacturer[64];
    char s_product[64];
};

struct razer_report {
    uint8_t status;
    uint8_t transaction_id;
    uint16_t remaining_packets;
    uint8_t protocol_type;
    uint8_t args_len;
    uint8_t cmd_class;
    uint8_t cmd_id;
    uint8_t args[80];
    uint8_t crc;
    uint8_t __reserved;
};

/* prepares a device for being interacted with */
int razer_init(struct razer_device *dev);

/* releases all resources in use for the specified device */
int razer_deinit(struct razer_device *dev);

/* obtains a list of supported Razer devices */
ssize_t razer_get_devices(struct razer_device **rzdevs);

/* releases resources allocated used by razer_get_devices() */
int razer_free_devices(struct razer_device **rzdevs);

/* sends a Razer command to the specified device */
int razer_send_report(struct razer_device *dev,
                      uint8_t cmd_class,
                      uint8_t cmd_id,
                      uint8_t *args,
                      uint8_t args_len);