#ifndef PM_CONF_H
#define PM_CONF_H

#include <stdint.h>
#define MAX_DISPLAYS 8

#include <alpm.h>

typedef struct __display_t {
  char* name;
  char* init_frag;
  char* state_frag;
  char* display_frag;
  uint16_t tps;
  uint16_t horizontal;
  uint16_t vertical;
  uint64_t cycles;
  uint16_t frames_per_tick;
} display_t;

typedef struct __config_t {
  display_t *display;
} config_t;


int parseconfig(const char *file, config_t *config);

#endif /* PM_CONF_H */
