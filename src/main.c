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

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#include <paper.h>

static void print_usage(char** argv) {
	char* slash = strrchr(argv[0], '/');
	uint64_t offset;
	if(slash == NULL) {
		offset = 0;
	} else {
		offset = (slash - argv[0]) + 1;
	}
	printf("%s [options] <output> <shader>\n", argv[0] + offset);
	printf("Options:\n");
	printf("--help\t\t-h\tDisplays this help message\n");
	printf("--fork\t\t-F\tForks glpaper so you can close the terminal\n");
	printf("--fps\t\t-f\tSets the FPS to render at\n");
	printf("--layer\t\t-l\tSpecifies layer to run on\n");
	printf("--width\t\t-W\tThe width to render at, this does not affect display resolution\n");
	printf("--height\t-H\tThe height to render at, this does not affect display resolution\n");
	exit(0);
}

int main(int argc, char** argv) {
	if(argc > 2) {
		struct option opts[] = {
			{
				.name = "help",
				.has_arg = no_argument,
				.flag = NULL,
				.val = 'h'
			},
			{
				.name = "fork",
				.has_arg = no_argument,
				.flag = NULL,
				.val = 'F'
			},
			{
				.name = "fps",
				.has_arg = required_argument,
				.flag = NULL,
				.val = 'f'
			},
			{
				.name = "layer",
				.has_arg = required_argument,
				.flag = NULL,
				.val = 'l'
			},
			{
				.name = "width",
				.has_arg = required_argument,
				.flag = NULL,
				.val = 'W'
			},
			{
				.name = "height",
				.has_arg = required_argument,
				.flag = NULL,
				.val = 'H'
			},
			{
				.name = NULL,
				.has_arg = 0,
				.flag = NULL,
				.val = 0
			}
		};
		char* fps_str = NULL;
		char* layer = NULL;
		char* width_str = NULL;
		char* height_str = NULL;
		char opt;
		while((opt = getopt_long(argc, argv, "hFf:l:W:H:", opts, NULL)) != -1) {
			switch(opt) {
			case 'h':
				print_usage(argv);
				break;
			case 'F':
				if(fork() > 0) {
					exit(0);
				}
				fclose(stdout);
				fclose(stderr);
				fclose(stdin);
				break;
			case 'f':
				fps_str = optarg;
				break;
			case 'l':
				layer = optarg;
				break;
			case 'W':
				width_str = optarg;
				break;
			case 'H':
				height_str = optarg;
				break;
			}
		}
		uint16_t fps;
		if(fps_str == NULL) {
			fps = 0;
		} else {
			fps = strtol(fps_str, NULL, 10);
		}

		uint16_t width;
		if(width_str == NULL) {
			width = 0;
		} else {
			width = strtol(width_str, NULL, 10);
		}

		uint16_t height;
		if(height_str == NULL) {
			height = 0;
		} else {
			height = strtol(height_str, NULL, 10);
		}

		if(optind + 1 >= argc) {
			print_usage(argv);
		}

		paper_init(argv[optind], argv[optind + 1], fps, layer, width, height);
	} else {
		print_usage(argv);
	}
}
