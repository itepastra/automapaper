#ifndef PM_CONF_H
#define PM_CONF_H

#include <stdint.h>
#define MAX_PATH_LENGTH 200
#define MAX_DISPLAY_LENGTH 10

#include <alpm.h>

typedef struct __display_t {
  char name[MAX_DISPLAY_LENGTH];
  char init_frag[MAX_PATH_LENGTH];
  char state_frag[MAX_PATH_LENGTH];
  char display_frag[MAX_PATH_LENGTH];
  uint16_t tps;
  uint16_t horizontal;
  uint16_t vertical;
  uint64_t cycles;
  uint16_t frames_per_tick;
} display;


int parseconfig(const char *file, display *config);

#endif /* PM_CONF_H */
