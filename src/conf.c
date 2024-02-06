#include "conf.h"
#include "toml.h"
#include <stdio.h>


int parseconfig(const char* file, config_t* config) {
  FILE* fp;
  char errbuf[200];

  fp = fopen(file, "r");
  if (!fp) {
    fprintf(stderr, "cannot open file %s\n", file);
    return 1;
  }

  toml_table_t* conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
  fclose(fp);
  if(!config) {
    fprintf(stderr, "cannot parse - %s\n", errbuf);
    toml_free(conf);
    return 1;
  }

  toml_array_t* displays = toml_array_in(conf, "display");

  int displays_len = toml_array_nelem(displays);

  printf("found %d displays\n", displays_len);
  config->amount = displays_len;

  for (int i = 0; i < displays_len; i++) {
    toml_table_t* display = toml_table_at(displays, i);

    int problem = 0;

    toml_datum_t name = toml_string_in(display, "name");
    if (!name.ok) {
      fprintf(stderr, "name value error in display %d\n", i);
      problem++;
    }
    toml_datum_t horizontal = toml_int_in(display, "horizontal");
    if (!horizontal.ok) {
      fprintf(stderr, "horizontal value error in display %d\n", i);
      problem++;
    }
    toml_datum_t vertical = toml_int_in(display, "vertical");
    if (!vertical.ok) {
      fprintf(stderr, "vertical value error in display %d\n", i);
      problem++;
    }
    toml_datum_t tps = toml_int_in(display, "tps");
    if (!tps.ok) {
      fprintf(stderr, "tps value error in display %d\n", i);
      problem++;
    }
    toml_datum_t state_frag = toml_string_in(display, "state_frag");
    if (!state_frag.ok) {
      fprintf(stderr, "state_frag value error in display %d\n", i);
      problem++;
    }
    toml_datum_t init_frag = toml_string_in(display, "init_frag");
    if (!init_frag.ok) {
      fprintf(stderr, "init_frag value error in display %d\n", i);
      problem++;
    }
    toml_datum_t display_frag = toml_string_in(display, "display_frag");
    if (!display_frag.ok) {
      fprintf(stderr, "display_frag value error in display %d\n", i);
      problem++;
    }

    if (!problem) {
      display_t current = { 
        .name = name.u.s,
        .horizontal = horizontal.u.b,
        .vertical = vertical.u.b,
        .tps = tps.u.b,
        .state_frag = state_frag.u.s,
        .init_frag = init_frag.u.s,
        .display_frag = display_frag.u.s,
      };
      printf("parsed display %d with name %s\n", i, current.name);
      config->displays[i] = &current;
    }
  }

  return 0;

}
