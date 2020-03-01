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
#include <unistd.h>

#include "razer.h"
#include "util.h"

int razer_get_brightness(struct razer_device *dev)
{
    int retval;
    uint8_t args[3] = {
        1, /* persist value? */
        LED_LOGO, /* LED ID */
        0 /* brightness */
    };

    /* get logo brightness */
    retval = razer_send_report(dev, 0x03, 0x83, args, sizeof(args), args, sizeof(args));
    if (retval < 0)
        return retval;
    printf("%d\n", args[2]);
    return args[2];
}

int razer_set_brightness(struct razer_device *dev, uint8_t brightness, int fade)
{
    uint8_t v, delta;
    uint8_t args[3] = {
        !fade,      /* persist value? only if not fading */
        0,          /* LED ID */
        brightness, /* brightness value */
    };

    if (!fade) {
        args[1] = LED_SCROLL;
        razer_send_report(dev, 0x03, 0x03, args, sizeof(args), NULL, 0);
        args[1] = LED_LOGO;
        razer_send_report(dev, 0x03, 0x03, args, sizeof(args), NULL, 0);
        return 0;
    }

    /* get initial brightness */
    v = razer_get_brightness(dev);
    /* fade the brightness up or down */
    delta = v < brightness ? +1 : -1;

    while (v != brightness) {
        v += delta;
        /* only persist intended brightness in order to avoid unneeded flash
         * writes on the device */
        if (v == brightness)
            args[0] = 1;
        args[2] = v;
        args[1] = LED_SCROLL;
        razer_send_report(dev, 0x03, 0x03, args, sizeof(args), NULL, 0);
        args[1] = LED_LOGO;
        razer_send_report(dev, 0x03, 0x03, args, sizeof(args), NULL, 0);
    }
    return 0;
}

int set_brightness(int verbose, int brightness, int fade)
{
    struct razer_device *devs;
    int ndevs, i;

    if (brightness < 0x00 || brightness > 0xFF) {
        fprintf(stderr, PR_ERROR "brightness must be between 0 and 255\n");
        return 1;
    }

    if (libusb_init(NULL) < 0) {
        fprintf(stderr, PR_ERROR "could not initialize libusb\n");
        return 1;
    }
    if (verbose)
        libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);

    ndevs = razer_get_devices(&devs);
    if (ndevs < 0) {
        fprintf(stderr, PR_ERROR "could not get Razer devices\n");
        return 1;
    }

    for (i = 0; i < ndevs; i++) {
        razer_init(&devs[i]);
        razer_set_brightness(&devs[i], brightness, fade);
        razer_deinit(&devs[i]);
    }
    razer_free_devices(&devs);
    libusb_exit(NULL);
    return 0;
}
