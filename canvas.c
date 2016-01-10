#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include "canvas.h"

static ALLEGRO_FONT* font;
static ALLEGRO_BITMAP *bm_terrains[MAXTERRAINS];
ALLEGRO_DISPLAY* display;
static int frame;

int canvas_init(void) {
    al_init();
    al_init_font_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_install_mouse();

    display = al_create_display(800, 600);
    al_set_window_title(display, "Eressea Allegro Client");
    font = al_create_builtin_font();
    al_clear_to_color(al_map_rgb(frame % 256, (frame >> 2) % 256, (frame >> 4) % 256));
    return 0;
}

int canvas_load_bmp(int index, const char * filename) {
    bm_terrains[index] = al_load_bitmap(filename);
    if (!bm_terrains[index]) {
        return -1;
    }
    return 0;
}

void canvas_done(void) {

}

void canvas_draw_hex(int sx, int sy, int index) {
    ALLEGRO_BITMAP *bm_hex = bm_terrains[index];
    al_draw_bitmap(bm_hex, sx, sy, 0);
}

void canvas_flip(void) {
    ++frame;
    al_flip_display();
    al_clear_to_color(al_map_rgb(frame % 256, (frame >> 2) % 256, (frame >> 4) % 256));
    al_draw_text(font, al_map_rgb(255, 255, 255), 400, 300, ALLEGRO_ALIGN_CENTER, "Welcome to Eressea!");
}

int canvas_get_refresh_rate() {
    int refresh_rate = al_get_display_refresh_rate(display);
    if (refresh_rate == 0) refresh_rate = 30;
    return refresh_rate;
}
