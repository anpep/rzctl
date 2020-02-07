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
#include <stdlib.h>

#include "util.h"
#include "3rdparty/argtable3.h"
#include "list_devices.h"
#include "brightness.h"
#include "effect.h"
#include "color.h"

#define PROGVERSION "0.1.0"

static void
display_help(const char *progname, void **argtables[], int nargtables,
             int extended)
{
    if (extended) {
        puts("This programs adds support for advanced features of\n"
             "Razer mice, such as coloring and LED effects.");
    }
    for (int i = 0; i < nargtables; i++) {
        printf("%s %s", i == 0 ? "usage:" : "      ", progname);
        arg_print_syntax(stdout, argtables[i], "\n");
        if (extended) {
            arg_print_glossary(stdout, argtables[i], "      %-20s %s\n");
            puts("");
        }
    }
}

static int
no_command(int help, int version, const char *progname, void **argtables[],
           int nargtables)
{
    if (help) {
        display_help(progname, argtables, nargtables, 1);
        return 0;
    }

    if (version) {
        printf("%s %s\n", progname, PROGVERSION);
        puts("Copyright (c) 2019 Angel. All rights reserved.\n"
             "This is free software; see the source for copying conditions.\n"
             "There is NO WARRANTY; not even for MERCHANTABILITY or FITNESS\n"
             "FOR A PARTICULAR PURPOSE.\n");
        return 0;
    }

    fprintf(stderr,
            PR_ERROR "no command provided. Try `%s --help' for more information\n",
            progname);
    return 1;
}

int main(int argc, char *argv[])
{
    unsigned i;
    int exit_code = 0;

    /* list-devices [-v] */
    struct arg_rex *ld_cmd = arg_rex1(NULL, NULL, "list-devices", NULL, 0,
                                      NULL);
    struct arg_lit *ld_verbose = arg_lit0("v", "verbose", "verbose messages");
    struct arg_end *ld_end = arg_end(20);
    void *ld_argtable[] = {ld_cmd, ld_verbose, ld_end};

    /* set-brightness <brightness> [-v] [-f] */
    struct arg_rex *sb_cmd = arg_rex1(NULL, NULL, "set-brightness", NULL, 0,
                                      NULL);
    struct arg_int *sb_brightness = arg_int1(NULL, NULL, "<brightness>",
                                             "brightness value (0-255)");
    struct arg_lit *sb_verbose = arg_lit0("v", "verbose", "verbose messages");
    struct arg_lit *sb_fade = arg_lit0("f", "fade", "fades the brightness up/down");
    struct arg_end *sb_end = arg_end(20);
    void *sb_argtable[] = {sb_cmd, sb_brightness, sb_verbose, sb_fade, sb_end};

    /* set-color [-v] */
    struct arg_rex *sc_cmd = arg_rex1(NULL, NULL, "set-color", NULL, 0,
                                      NULL);
    struct arg_str *sc_color = arg_str1(NULL, NULL, "<color>",
                                        "hexadecimal color value (e.g. ffffff)");
    struct arg_lit *sc_verbose = arg_lit0("v", "verbose", "verbose messages");
    struct arg_end *sc_end = arg_end(20);
    void *sc_argtable[] = {sc_cmd, sc_color, sc_verbose, sc_end};

    /* set-effect [-v] */
    struct arg_rex *se_cmd = arg_rex1(NULL, NULL, "set-effect", NULL, 0,
                                      NULL);
    struct arg_str *se_effect = arg_str1(NULL, NULL, "<solid|pulse|spectrum>",
                                         "LED effect");
    struct arg_lit *se_verbose = arg_lit0("v", "verbose", "verbose messages");
    struct arg_end *se_end = arg_end(20);
    void *se_argtable[] = {se_cmd, se_effect, se_verbose, se_end};

    /* (no command) [-h|--help] [--version] */
    struct arg_lit *nc_help = arg_lit0("h", "help", "print this help and exit");
    struct arg_lit *nc_version = arg_lit0(NULL, "version",
                                          "print version information and exit");
    struct arg_end *nc_end = arg_end(20);
    void *nc_argtable[] = {nc_help, nc_version, nc_end};

    void **argtables[] = {ld_argtable, sb_argtable, sc_argtable, se_argtable,
                          nc_argtable};

    for (i = 0; i < sizeof(argtables) / sizeof(argtables[0]); i++) {
        if (arg_nullcheck(argtables[i])) {
            fprintf(stderr, PR_ERROR ": insufficient memory\n");
            exit(1);
        }
    }

    if (arg_parse(argc, argv, ld_argtable) == 0) {
        exit_code = list_devices(ld_verbose->count);
    } else if (arg_parse(argc, argv, nc_argtable) == 0) {
        exit_code = no_command(nc_help->count, nc_version->count, argv[0],
                               argtables,
                               sizeof(argtables) / sizeof(argtables[0]));
    } else if (arg_parse(argc, argv, sb_argtable) == 0) {
        exit_code = set_brightness(sb_verbose->count, sb_brightness->ival[0], sb_fade->count);
    } else if (arg_parse(argc, argv, sc_argtable) == 0) {
        exit_code = set_color(sc_verbose->count, sc_color->sval[0]);
    } else if (arg_parse(argc, argv, se_argtable) == 0) {
        exit_code = set_effect(se_verbose->count, se_effect->sval[0]);
    } else {
        display_help(argv[0], argtables,
                     sizeof(argtables) / sizeof(argtables[0]), 0);
        exit_code = 1;
    }

    return exit_code;
}