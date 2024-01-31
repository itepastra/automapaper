/*
 *  Copyright (C) 2019 Scoopta
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

#include <utils.h>

time_t utils_get_time_millis() {
	struct timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

void utils_sleep_millis(time_t millis) {
	struct timespec time;
	time.tv_sec = millis / 1000;
	time.tv_nsec = (millis % 1000) * pow(1000, 2);
	nanosleep(&time, NULL);
}
