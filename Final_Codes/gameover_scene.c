#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "leaderboard_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static ALLEGRO_BITMAP* gameOverbg;
static ALLEGRO_FONT* youDiedFont;
static ALLEGRO_FONT* shikaFont;
static Button mainmenu;
static Button highscore;
int tick;
int curLevel;
bool newHighscore;
bool canAddHighscore;

static void init(void) {
    tick = 0;
    youDiedFont = al_load_font("Assets/YouDiedFont.otf", 64, 0);
    shikaFont = al_load_font("Assets/YouDiedFont.otf", 32, 0);
    mainmenu = button_create(SCREEN_W / 2 - 150, 600, 300, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    highscore = button_create(SCREEN_W/2 - 150, 450, 300, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    change_bgm("None", true, AUDIO_FADE_TIME);
    int min, max;
    getMinMaxScore(&min, &max);
    newHighscore = curLevel > max;
    canAddHighscore = curLevel > min;
    change_bgm("Assets/audio/losing_music.ogg", true, AUDIO_FADE_TIME);
}

static void update(void) {
    update_button(&mainmenu);
    if (canAddHighscore) update_button(&highscore);
    if (mainmenu.hovered && mouseButtonUp & 1) {
        change_scene(create_menu_scene(), SCENE_FADE_TIME);
    }
    if (highscore.hovered && mouseButtonUp & 1) {
        change_scene(create_leaderboard_scene(curLevel), SCENE_FADE_TIME);
    }
    tick++;
}

static void draw(void) {
    int YouDiedOpacity = number_map(0, 120, 0, 200, tick);
    int BgOpacity = number_map(30, 120, 255, 60, tick);
    int ShikaOpacity = number_map(120, 240, 0, 200, tick);
    al_draw_tinted_bitmap(gameOverbg, al_map_rgba(255, 255, 255, BgOpacity), 0, 0, 0);
    al_draw_text(youDiedFont, al_map_rgba(201, 8, 3, YouDiedOpacity), SCREEN_W / 2, 200, ALLEGRO_ALIGN_CENTER, "YOU DIER");
    al_draw_text(shikaFont, al_map_rgba(255, 255, 255, ShikaOpacity), SCREEN_W / 2, 300, ALLEGRO_ALIGN_CENTER, newHighscore ? "HIGH SCORE" : "Nice try");
    if (tick > 240) {
        draw_button(mainmenu, "Main menu");
        if (canAddHighscore)
            draw_button(highscore, "Leaderboard");
    }
}

static void destroy(void) {
    al_destroy_bitmap(gameOverbg);
    al_destroy_font(youDiedFont);
    al_destroy_font(shikaFont);
    destroy_button(&highscore);
    destroy_button(&mainmenu);
}


Scene create_gameover_scene(ALLEGRO_BITMAP* background, int currentLevel) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "gameover";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;
    gameOverbg = al_clone_bitmap(background);
    curLevel = currentLevel;
    return scene;
}
