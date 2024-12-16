#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "loading_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static Button settingButton;
static Button startButton;

static void init(void) {
    settingButton = button_create(SCREEN_W / 2 - 200, 600, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    startButton = button_create(SCREEN_W / 2 - 200 , 450, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    change_bgm("Assets/audio/menu_bgm.mp3");
}

static void update(void) {
    update_button(&settingButton);
    update_button(&startButton); 
    if (settingButton.hovered && mouseState.buttons) {
        change_scene(create_setting_scene());
    }
    if (startButton.hovered && mouseState.buttons) {
        change_scene(create_game_scene());
    }

    /*
        [TODO HACKATHON 4-3] 
        
        Change scene to setting scene when the button is pressed
    */
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
    // button
    draw_button(settingButton, "SETTING");
    // button text
}

static void destroy(void) {
    destroy_button(&settingButton);
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
