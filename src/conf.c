#include "conf.h"
#include "toml.h"
#include <stdio.h>

int parseconfig(const char *file, config_t *config) {
  FILE *fp;
  char errbuf[200];

  fp = fopen(file, "r");
  if (!fp) {
    fprintf(stderr, "cannot open file %s\n", file);
    return 1;
  }

  toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
  fclose(fp);
  if (!config) {
    fprintf(stderr, "cannot parse - %s\n", errbuf);
    toml_free(conf);
    return 1;
  }

  toml_table_t *display = toml_table_in(conf, "display");

  int problem = 0;

  toml_datum_t name = toml_string_in(display, "name");
  if (!name.ok) {
    fprintf(stderr, "name value error in display\n");
    problem++;
  }
  toml_datum_t horizontal = toml_int_in(display, "horizontal");
  if (!horizontal.ok) {
    fprintf(stderr, "horizontal value error in display\n");
    problem++;
  }
  toml_datum_t vertical = toml_int_in(display, "vertical");
  if (!vertical.ok) {
    fprintf(stderr, "vertical value error in display\n");
    problem++;
  }
  toml_datum_t tps = toml_int_in(display, "tps");
  if (!tps.ok) {
    fprintf(stderr, "tps value error in display\n");
    problem++;
  }
  toml_datum_t state_frag = toml_string_in(display, "state_frag");
  if (!state_frag.ok) {
    fprintf(stderr, "state_frag value error in display\n");
    problem++;
  }
  toml_datum_t init_frag = toml_string_in(display, "init_frag");
  if (!init_frag.ok) {
    fprintf(stderr, "init_frag value error in display\n");
    problem++;
  }
  toml_datum_t display_frag = toml_string_in(display, "display_frag");
  if (!display_frag.ok) {
    fprintf(stderr, "display_frag value error in display\n");
    problem++;
  }
  toml_datum_t cycles = toml_int_in(display, "cycles");
  if (!cycles.ok) {
    fprintf(stderr, "cycles value error in display\n");
    problem++;
  }
  toml_datum_t frame_skip = toml_int_in(display, "frames_per_tick");
  if (!frame_skip.ok) {
    fprintf(stderr, "frames_per_tick value error in display\n");
    problem++;
  }

  if (problem == 0) {
    display_t current = {
        .name = name.u.s,
        .horizontal = horizontal.u.b,
        .vertical = vertical.u.b,
        .tps = tps.u.b,
        .state_frag = state_frag.u.s,
        .init_frag = init_frag.u.s,
        .display_frag = display_frag.u.s,
        .frames_per_tick = frame_skip.u.b,
        .cycles = cycles.u.i,
    };
    printf("parsed display with name %s\n", current.name);
    config->display = &current;
    return 0;
  }

  return 1;
}
