#ifndef PM_CONF_H
#define PM_CONF_H

#define MAX_DISPLAYS 8

#include <alpm.h>

typedef struct __display_t {
  char* name;
  char* init_frag;
  char* state_frag;
  char* display_frag;
  int tps;
  int horizontal;
  int vertical;
} display_t;

typedef struct __config_t {
  size_t amount;
  display_t *displays[MAX_DISPLAYS];
} config_t;


int parseconfig(const char *file, config_t *config);

#endif /* PM_CONF_H */
