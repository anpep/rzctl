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

#define _DEFAULT_SOURCE /* required for usleep */

#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>
#include <assert.h>
#include <unistd.h>

#include "razer.h"

static const uint16_t k_supported_vid = 0x1532;
static const uint16_t k_supported_pids[] = { /* please keep this array sorted */
        0x0043 /* Razer Deathadder Chroma */
};

/* comparison function for bsearch() */
static int __attribute__((noinline))
search_cmpfn(const void *a, const void *b)
{
    /* comparison succeeds if a == b */
    return *(uint16_t *) a - *(uint16_t *) b;
}

static uint8_t
report_crc(struct razer_report *report)
{
    uint8_t i;
    uint8_t crc = 0;

    for (i = 2; i < sizeof(struct razer_report) - 2; i++)
        crc ^= ((uint8_t *) report)[i];
    return crc;
}

int razer_init(struct razer_device *dev)
{
    int iface;

    if (libusb_open(dev->dev, &(dev->handle)) < 0)
        return -1;
    if (libusb_get_config_descriptor(dev->dev, 0, &(dev->cfg_desc)) < 0)
        return -1;
    /* detach kernel drivers associated with interfaces that are to be claimed
     * by rzctl */
    libusb_set_auto_detach_kernel_driver(dev->handle, 1);
    /* claim all available interfaces */
    for (iface = 0; iface < dev->cfg_desc->bNumInterfaces; iface++)
        libusb_claim_interface(dev->handle, iface);

    /* obtain manufacturer and product name strings */
    libusb_get_string_descriptor_ascii(dev->handle, dev->desc.iManufacturer,
                                       (unsigned char *) dev->s_manufacturer,
                                       sizeof(dev->s_manufacturer));
    libusb_get_string_descriptor_ascii(dev->handle, dev->desc.iProduct,
                                       (unsigned char *) dev->s_product,
                                       sizeof(dev->s_product));
    return 0;
}

int razer_deinit(struct razer_device *dev)
{
    int iface;

    /* release claimed interfaces */
    for (iface = 0; iface < dev->cfg_desc->bNumInterfaces; iface++)
        libusb_release_interface(dev->handle, iface);
    libusb_free_config_descriptor(dev->cfg_desc);
    libusb_close(dev->handle);
    return 0;
}

/* the caller is responsible of freeing the allocated structure by using the
 * razer_free_devices() function */
ssize_t razer_get_devices(struct razer_device **rzdevs)
{
    ssize_t ndevs, ndevs_supported = 0;
    libusb_device **devs;
    int i;

    ndevs = libusb_get_device_list(NULL, &devs);
    if (ndevs < 1)
        return ndevs;

    /* allocate space for the array of Razer PIDs */
    *rzdevs = malloc(sizeof(struct razer_device) * ndevs);
    for (i = 0; i < ndevs; i++) {
        struct razer_device *rzdev = (struct razer_device *) (*rzdevs +
                                                              sizeof(struct razer_device) *
                                                              ndevs_supported);
        memset(rzdev, 0, sizeof(struct razer_device));
        if (libusb_get_device_descriptor(devs[i], &(rzdev->desc)) < 0)
            continue;
        if (rzdev->desc.idVendor != k_supported_vid)
            continue;

        /* look for the product ID on the supported PIDs array */
        if (bsearch(&(rzdev->desc.idProduct), k_supported_pids,
                    sizeof(k_supported_pids) / sizeof(uint16_t),
                    sizeof(uint16_t), search_cmpfn) == NULL)
            continue;

        /* this device is supported */
        rzdev->dev = devs[i];
        ndevs_supported++;
    }
    libusb_free_device_list(devs, 1);
    *rzdevs = realloc(*rzdevs, sizeof(struct razer_device) * ndevs_supported);
    return ndevs_supported;
}

int razer_free_devices(struct razer_device **rzdevs)
{
    if (!rzdevs || !(*rzdevs))
        return -1;
    free(*rzdevs);
    *rzdevs = NULL;
    return 0;
}

int razer_send_report(struct razer_device *dev,
                      uint8_t cmd_class,
                      uint8_t cmd_id,
                      uint8_t *in_args,
                      uint8_t in_args_len,
                      uint8_t *out_args,
                      uint8_t out_args_len)
{
    int retval;
    uint8_t transaction_id;
    struct razer_report report = {0};

    transaction_id = rand() % 0xff;
    report.transaction_id = transaction_id;
    report.cmd_class = cmd_class;
    report.cmd_id = cmd_id;
    report.args_len = in_args_len;
    memcpy(report.args, in_args, in_args_len);
    report.crc = report_crc(&report);

    /* send feature report */
    do {
        retval = libusb_control_transfer(
                dev->handle,
                LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS |
                LIBUSB_RECIPIENT_INTERFACE,
                HID_SET_REPORT,
                (HID_REPORT_TYPE_FEATURE << 8) | 0x00,
                0x0000,
                (uint8_t *) &report,
                sizeof(struct razer_report),
                REPORT_TIMEOUT);
    } while (retval < 0);

    /* HACK: actually wait for the device firmware to be ready */
    usleep(REPORT_TIMEOUT);
    if (!out_args || !out_args_len)
        return retval;

    /* obtain response report from device */
    do {
        retval = libusb_control_transfer(
                dev->handle,
                LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS |
                LIBUSB_RECIPIENT_INTERFACE,
                HID_GET_REPORT,
                (HID_REPORT_TYPE_FEATURE << 8) | 0x00,
                0x0000,
                (uint8_t *) &report,
                sizeof(struct razer_report),
                REPORT_TIMEOUT);
    } while (retval < 0);

    /* copy response arguments, without overflowing the supplied buffer */
    retval = report.args_len > out_args_len ? out_args_len : report.args_len;
    memcpy(out_args, report.args, retval);

    /* TODO: handle out-of-order input reports and correlate using transaction_id */
    assert(report.transaction_id == transaction_id);
    return retval;
}
