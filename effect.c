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

#include <string.h>
#include <stdio.h>

#include "effect.h"
#include "util.h"
#include "list_devices.h"

int razer_set_effect(struct razer_device *dev, int effect)
{
    uint8_t args[3] = {
        1, /* persist value? */
        0, /* LED ID */
        effect
    };

    /* set logo effect */
    args[1] = LED_SCROLL;
    razer_send_report(dev, 0x03, 0x02, args, sizeof(args), NULL, 0);
    /* set scroll wheel effect */
    args[1] = LED_LOGO;
    razer_send_report(dev, 0x03, 0x02, args, sizeof(args), NULL, 0);
    return 0;
}

int set_effect(int verbose, const char *s_effect)
{
    int effect;
    struct razer_device *devs;
    int ndevs, i;

    if (!strcmp("solid", s_effect)) {
        effect = EFFECT_SOLID;
    } else if (!strcmp("pulse", s_effect)) {
        effect = EFFECT_PULSE;
    } else if (!strcmp("spectrum", s_effect)) {
        effect = EFFECT_SPECTRUM;
    } else {
        fprintf(stderr, PR_ERROR "invalid effect name: `%s'\n", s_effect);
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
        razer_set_effect(&devs[i], effect);
        razer_deinit(&devs[i]);
    }
    razer_free_devices(&devs);
    libusb_exit(NULL);
    return 0;
}
