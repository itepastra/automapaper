#include "conf.h"
#include "toml.h"
#include <stdio.h>
#include <string.h>
#include <wordexp.h>

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
    display->frames_per_tick = frame_skip.u.b, display->cycles = cycles.u.i;
    wordexp_t init_exp;
    if (wordexp(init_frag.u.s, &init_exp, 0) == 0) {
      for (size_t i = 0; i < init_exp.we_wordc; i++) {
        strcpy(display->init_frag, init_exp.we_wordv[i]);
        free(init_frag.u.s);
      }
      wordfree(&init_exp);
    } else {
      // Handle error
      fprintf(stderr, "Error expanding init_frag file path\n");
      return 1;
    }

    wordexp_t state_exp;
    if (wordexp(state_frag.u.s, &state_exp, 0) == 0) {
      for (size_t i = 0; i < state_exp.we_wordc; i++) {
        strcpy(display->state_frag, state_exp.we_wordv[i]);
        free(state_frag.u.s);
      }
      wordfree(&state_exp);
    } else {
      // Handle error
      fprintf(stderr, "Error expanding state_frag file path\n");
      return 1;
    }

    wordexp_t display_exp;
    if (wordexp(display_frag.u.s, &display_exp, 0) == 0) {
      for (size_t i = 0; i < display_exp.we_wordc; i++) {
        strcpy(display->display_frag, display_exp.we_wordv[i]);
        free(display_frag.u.s);
      }
      wordfree(&display_exp);
    } else {
      // Handle error
      fprintf(stderr, "Error expanding display_frag file path\n");
      return 1;
    }
    printf("parsed display with name %s\n", display->name);
    return 0;
  }
  return 1;
}
