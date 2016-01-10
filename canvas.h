#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define MAXTERRAINS 8

int canvas_init(void);
int canvas_load_bmp(int index, const char * filename);
void canvas_done(void);
void canvas_draw_hex(int sx, int sy, int index);
void canvas_flip(void);
int canvas_get_refresh_rate();

extern ALLEGRO_DISPLAY* display;

#ifdef __cplusplus
}
#endif
