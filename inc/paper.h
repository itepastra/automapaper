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

#ifndef PAPER_H
#define PAPER_H

#include "conf.h"

void paper_init(const char *layer_name, const display_t display_config);

void paper_run(char *_monitor, char *init_path, char *state_path,
               char *display_path, const uint16_t fps, const char *layer_name,
               const uint16_t width, const uint16_t height, const uint64_t cycles, const uint16_t frame_skip);
#endif
