/*
 *  Copyright (C) 2019-2020 Scoopta
 *  This file is part of GLPaper
 *  GLPaper is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GLPaper is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GLPaper.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "conf.h"
#include "ini.h"
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <paper.h>

static void print_usage(char **argv) {
  char *slash = strrchr(argv[0], '/');
  uint64_t offset;
  if (slash == NULL) {
    offset = 0;
  } else {
    offset = (slash - argv[0]) + 1;
  }
  printf("%s [options] <output> <shader>\n", argv[0] + offset);
  printf("Options:\n");
  printf("--help\t\t-h\tDisplays this help message\n");
  printf("--fork\t\t-F\tForks glpaper so you can close the terminal\n");
  printf("--layer\t\t-l\tSpecifies layer to run on\n");
  printf("--config\t\t-C\tSpecifies the config to use\n");
  exit(0);
}

int main(int argc, char **argv) {
  if (argc > 0) {
    struct option opts[] = {
        {.name = "help", .has_arg = no_argument, .flag = NULL, .val = 'h'},
        {.name = "fork", .has_arg = no_argument, .flag = NULL, .val = 'F'},
        {.name = "layer",
         .has_arg = required_argument,
         .flag = NULL,
         .val = 'l'},
        {.name = "config",
         .has_arg = required_argument,
         .flag = NULL,
         .val = 'C'},
        {.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}};
    char *layer = NULL;
    char *config_str = NULL;
    char opt;
    while ((opt = getopt_long(argc, argv, "hFf:l:W:H:C:", opts, NULL)) != -1) {
      switch (opt) {
      case 'h':
        print_usage(argv);
        break;
      case 'F':
        if (fork() > 0) {
          exit(0);
        }
        fclose(stdout);
        fclose(stderr);
        fclose(stdin);
        break;
      case 'l':
        layer = optarg;
        break;
      case 'C':
        config_str = optarg;
        break;
      }
    }

    if (config_str == NULL) {
      char *config = getenv("XDG_CONFIG_HOME");
      if (config == NULL) {
        char *home = getenv("HOME");
        if (home == NULL) {
          fprintf(stderr, "HOME environment variable not set, please specify a "
                          "config file to use");
        }
        config_str = strcat(getenv("HOME"), "/.config/automapaper/config.toml");
      } else {
        config_str = strcat(config, "/automapaper/config.toml");
      }
      printf("trying to find config at %s\n", config_str);
    }

    display config;
    int parseresult = parseconfig(config_str, &config);
    if (parseresult != 0) {
      fprintf(stderr, "parsing had an error, exiting.\n");
      exit(parseresult);
    }
    paper_init(layer, config);
  } else {
    print_usage(argv);
  }
}
