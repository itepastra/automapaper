#include "conf.h"
#include "toml.h"
#include <stdio.h>
#include <string.h>

int parseconfig(const char *file, display *display) {
  FILE *fp;
  char errbuf[200];

  fp = fopen(file, "r");
  if (!fp) {
    fprintf(stderr, "cannot open file %s\n", file);
    return 1;
  }

  toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
  fclose(fp);
  if (!display) {
    fprintf(stderr, "cannot parse - %s\n", errbuf);
    toml_free(conf);
    return 1;
  }

  toml_table_t *displ = toml_table_in(conf, "display");

  int problem = 0;

  toml_datum_t name = toml_string_in(displ, "name");
  if (!name.ok) {
    fprintf(stderr, "name value error in display\n");
    problem++;
  }
  toml_datum_t horizontal = toml_int_in(displ, "horizontal");
  if (!horizontal.ok) {
    fprintf(stderr, "horizontal value error in display\n");
    problem++;
  }
  toml_datum_t vertical = toml_int_in(displ, "vertical");
  if (!vertical.ok) {
    fprintf(stderr, "vertical value error in display\n");
    problem++;
  }
  toml_datum_t tps = toml_int_in(displ, "tps");
  if (!tps.ok) {
    fprintf(stderr, "tps value error in display\n");
    problem++;
  }
  toml_datum_t state_frag = toml_string_in(displ, "state_frag");
  if (!state_frag.ok) {
    fprintf(stderr, "state_frag value error in display\n");
    problem++;
  }
  toml_datum_t init_frag = toml_string_in(displ, "init_frag");
  if (!init_frag.ok) {
    fprintf(stderr, "init_frag value error in display\n");
    problem++;
  }
  toml_datum_t display_frag = toml_string_in(displ, "display_frag");
  if (!display_frag.ok) {
    fprintf(stderr, "display_frag value error in display\n");
    problem++;
  }
  toml_datum_t cycles = toml_int_in(displ, "cycles");
  if (!cycles.ok) {
    fprintf(stderr, "cycles value error in display\n");
    problem++;
  }
  toml_datum_t frame_skip = toml_int_in(displ, "frames_per_tick");
  if (!frame_skip.ok) {
    fprintf(stderr, "frames_per_tick value error in display\n");
    problem++;
  }

  if (problem == 0) {
    strcpy(display->name, name.u.s);
    free(name.u.s);
    display->horizontal = horizontal.u.b, display->vertical = vertical.u.b;
    display->tps = tps.u.b;
    strcpy(display->init_frag, init_frag.u.s);
    free(init_frag.u.s);
    strcpy(display->state_frag, state_frag.u.s);
    free(state_frag.u.s);
    strcpy(display->display_frag, display_frag.u.s);
    free(display_frag.u.s);
    display->frames_per_tick = frame_skip.u.b, display->cycles = cycles.u.i;
    printf("parsed display with name %s\n", display->name);
    return 0;
  }
  return 1;
}

