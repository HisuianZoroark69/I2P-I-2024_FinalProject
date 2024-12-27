#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "leaderboard_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"
#include "player.h"

static Button settingButton;
static Button startButton;
static Button leaderboard;
static PlayerStat defaultStat;

static void init(void) {
    settingButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    leaderboard = button_create(SCREEN_W / 2 - 200 , 525, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    startButton = button_create(SCREEN_W / 2 - 200, 405, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    change_bgm("Assets/audio/menu_music.mp3", true);
    defaultStat.atk = 10;
    defaultStat.atkSpd = 30;
    defaultStat.evasion = 0;
    defaultStat.health = 10;
    defaultStat.speed = 4;
}

static void update(void) {
    update_button(&settingButton);
    update_button(&startButton); 
    update_button(&leaderboard);
    if (settingButton.hovered && mouseButtonUp & 1) {
        change_scene(create_setting_scene());
    }
    if (startButton.hovered && mouseButtonUp & 1) {
        change_scene(create_game_scene(1, 0, defaultStat));
    }
    if (leaderboard.hovered && mouseButtonUp & 1) {
        change_scene(create_leaderboard_scene(0));
    }
}

static void draw(void) {
    // Title Text
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(146, 161, 185),
        SCREEN_W / 2,
        225,
        ALLEGRO_ALIGN_CENTER,
        "Tales Saga Chronicles 3"
    );
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(199, 207, 221),
        SCREEN_W / 2,
        220,
        ALLEGRO_ALIGN_CENTER,
        "Tales Saga Chronicles 3"
    );

    draw_button(startButton, "START");
    draw_button(leaderboard, "LEADERBOARD");
    draw_button(settingButton, "SETTING");
}

static void destroy(void) {
    destroy_button(&settingButton);
    destroy_button(&startButton);
    destroy_button(&leaderboard);
}


Scene create_menu_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "menu";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
