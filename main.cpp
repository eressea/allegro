#include <allegro5/allegro.h>
#include "canvas.h"
#include <cstring>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

struct terrain_data {
    const char * name;
    const char * crtoken;
} terrains[MAXTERRAINS] = {
    { "ocean", "Ozean" },
    { "plain", "Ebene" },
    { "desert", "W\xC3\xBCste" },
    { "highland", "Hochland" },
    { "mountain", "Berge" },
    { "firewall", "Feuerwand" },
    { "glacier", "Gletscher" },
    { "swamp", "Sumpf" } };

class HexTile {
public:
    HexTile(int x, int y, int terrain) : x(x), y(y), terrain(terrain) {}

    int x, y, terrain;
private:
    std::string name;
};

class HexMap {
public:
    typedef std::vector<HexTile *> row_type;
    typedef std::map<int, row_type> map_type;
    HexTile * addTile(int x, int y, int terrain) {
        HexTile * tile = new HexTile(x, y, terrain);
        m_tiles[y].push_back(tile);
        return tile;
    }
    HexTile * getTile(int x, int y) const {
        const row_type *tiles = getTiles(y);
        if (tiles) {
            for (auto tile : *tiles) {
                if (tile->x == x) return tile;
            }
        }
        return NULL;
    }
    const row_type * getTiles(int y) const {
        map_type::const_iterator it = m_tiles.find(y);
        if (it != m_tiles.end()) {
            return &(*it).second;
        }
        return 0;
    }
private:
    map_type m_tiles;
};

class Application {
public:
    Application() : m_drag(false), m_scroll_x(0), m_scroll_y(0) {}
    int run(void);
    void read_report(const char *filename);
    void init_map(void);
    void select_tile(int sx, int sy);

private:
    void paint(void);
    void update(double delta);

    HexMap hmap;
    int frame;
    bool m_drag;
    int m_scroll_x, m_scroll_y;
};

void Application::read_report(const char * filename) {
    FILE * F = fopen(filename, "r");
    int x, y, p, terrain = -1;
    if (!F) return;
    while (!feof(F)) {
        char buffer[256];
        char *tok;
        fgets(buffer, sizeof(buffer), F);
        tok = strtok(buffer, " ;\n");
        if (strcmp(tok, "REGION") == 0) {
            tok = strtok(NULL, " \n");
            if (tok) x = atoi(tok);
            tok = strtok(NULL, " \n");
            if (tok) y = atoi(tok);
            tok = strtok(NULL, " \n");
            p = tok ? atoi(tok) : 0;
        }
        else {
            tok = strtok(NULL, " ;\n");
            if (tok && strcmp(tok, "Terrain") == 0) {
                int i;
                for (i = 0; i != MAXTERRAINS; ++i) {
                    if (strncmp(buffer + 1, terrains[i].crtoken, strlen(terrains[i].crtoken)) == 0) {
                        terrain = i;
                        break;
                    }
                }
                if (terrain >= 0 && p == 0) {
                    hmap.addTile(x, y, terrain);
                }
            }
        }
    }
    fclose(F);
}

void Application::init_map(void) {
    int x, y;
    int width = 16;
    int height = 16;
    for (x = 1 - width; x != width; ++x) {
        for (y = 1 - height; y != height; ++y) {
            int terrain = 0;
            if (x == 0 && y == 0) terrain = 5;
            if (x == 4) terrain = y % MAXTERRAINS;
            if (y == 4) terrain = x % MAXTERRAINS;
            if (terrain < 0) terrain = (-terrain) % MAXTERRAINS;
            hmap.addTile(x, y, terrain);
        }
    }
}

void Application::paint(void) {
    int y, height;
    height = 800 / 48;
    for (y = 0; y != height; ++y) {
        int yof = m_scroll_y / 48 + 2; // TODO: yes, but why?
        const HexMap::row_type * tiles = hmap.getTiles(yof - y);
        if (tiles) {
            for (auto tile : *tiles) {
                int sx = tile->x, sy = tile->y;
                sx = m_scroll_x + 64 * sx + 32 * sy;
                sy = m_scroll_y - sy * 48;
                if (sx < 800 && sx > -80 && sy < 600 && sy > -80) {
                    canvas_draw_hex(sx, sy, tile->terrain);
                }
            }
        }
    }
    canvas_flip();
}

void Application::update(double delta) {
    ++frame;
}

void Application::select_tile(int sx, int sy) {
    HexTile *tile;
    int x, y;

    sx -= m_scroll_x + 8;
    sy -= m_scroll_y + 24;
    y = - sy / 48;
    x = (sx - 32 * y) / 64;
    tile = hmap.getTile(x, y);
    if (tile) {
        tile->terrain = (tile->terrain + 1) % MAXTERRAINS;
    }
}

int Application::run(void) {
    ALLEGRO_TIMER* timer;
    ALLEGRO_EVENT_QUEUE *queue;
    int refresh_rate, i;
    double delta;
    canvas_init();

    for (i = 0; i != MAXTERRAINS; ++i) {
        char filename[64];
        size_t size = sizeof(filename);
        strncpy(filename, "res/", size);
        strncat(filename, terrains[i].name, size);
        strncat(filename, ".png", size);
        if (canvas_load_bmp(i, filename)!=0) {
            fprintf(stderr, "could not load %s\n", filename);
        }
    }
    refresh_rate = canvas_get_refresh_rate();
    delta = 1.0 / refresh_rate;
    timer = al_create_timer(delta);
    al_start_timer(timer);

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, (ALLEGRO_EVENT_SOURCE*)al_get_keyboard_event_source());
    al_register_event_source(queue, (ALLEGRO_EVENT_SOURCE*)al_get_mouse_event_source());
    al_register_event_source(queue, (ALLEGRO_EVENT_SOURCE*)timer);

    delta = al_get_time();
    for (;;) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        switch (event.type) {
        case ALLEGRO_EVENT_KEY_DOWN:
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) return 0;
            else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) frame = 0;
            break;
        case ALLEGRO_EVENT_MOUSE_AXES:
            if (m_drag) {
                m_scroll_x += event.mouse.dx;
                m_scroll_y += event.mouse.dy;
            }
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            if (event.mouse.button == 2) m_drag = false;
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if (event.mouse.button == 2) m_drag = true;
            else if (event.mouse.button == 1) {
                select_tile(event.mouse.x, event.mouse.y);
            }
            break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            return 0;
        case ALLEGRO_EVENT_TIMER:
            delta = al_get_time() - delta;
            update(delta);
            paint();
            break;
        }
    }
    return 0;
}

int main(int argc, char ** argv)
{
    Application app;
    if (argc > 1) {
        app.read_report(argv[1]);
    }
    else {
        app.init_map();
    }
    return app.run();
}
