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

#include <stdio.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#include "list_devices.h"
#include "util.h"

int list_devices(int verbose)
{
    struct razer_device *devs;
    int ndevs, i;

    if (libusb_init(NULL) < 0) {
        fprintf(stderr, PR_ERROR "could not initialize libusb\n");
        return 1;
    }
    if (verbose)
        libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_DEBUG);

    ndevs = razer_get_devices(&devs);
    if (ndevs < 0) {
        fprintf(stderr, PR_ERROR "could not get Razer devices\n");
        return 1;
    }

    for (i = 0; i < ndevs; i++) {
        razer_init(&devs[i]);
        printf("%04x:%04x %s (%s)\n", devs[i].desc.idVendor, devs[i].desc.idProduct,
                devs[i].s_product, devs[i].s_manufacturer);
        razer_deinit(&devs[i]);
    }
    razer_free_devices(&devs);
    libusb_exit(NULL);
    return 0;
}
