#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static ALLEGRO_BITMAP* bg;
static ALLEGRO_FONT* youDiedFont;
static ALLEGRO_FONT* shikaFont;
static Button mainmenu;
int tick;

static void init(void) {
    tick = 0;
    youDiedFont = al_load_font("Assets/YouDiedFont.otf", 64, 0);
    shikaFont = al_load_font("Assets/YouDiedFont.otf", 32, 0);
    mainmenu = button_create(SCREEN_W/2 - 150, 600, 300, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    change_bgm("None");
}

static void update(void) {
    update_button(&mainmenu);
    if (mainmenu.hovered && mouseState.buttons & 1) {
        change_scene(create_menu_scene());
    }
    tick++;
}

static void draw(void) {
    int YouDiedOpacity = number_map(0, 120, 0, 200, tick);
    int BgOpacity = number_map(30, 120, 255, 100, tick);
    int ShikaOpacity = number_map(120, 240, 0, 200, tick);
    al_draw_tinted_bitmap(bg, al_map_rgba(255, 255, 255, BgOpacity), 0, 0, 0);
    al_draw_text(youDiedFont, al_map_rgba(201, 8, 3, YouDiedOpacity), SCREEN_W / 2, 200, ALLEGRO_ALIGN_CENTER, "YOU DEER");
    al_draw_text(shikaFont, al_map_rgba(255, 255, 255, ShikaOpacity), SCREEN_W / 2, 300, ALLEGRO_ALIGN_CENTER, "~SHIKA~");
    if(tick > 240)
        draw_button(mainmenu, "Main menu");
}

static void destroy(void) {
    al_destroy_bitmap(bg);
    al_destroy_font(youDiedFont);
    al_destroy_font(shikaFont);
}


Scene create_gameover_scene(ALLEGRO_BITMAP* background) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "loading";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;
    bg = al_clone_bitmap(background);
    return scene;
}
