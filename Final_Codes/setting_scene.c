#include <allegro5/allegro.h>
#include "setting_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static Button backButton;
static Slider bgmSlider, sfxSlider;


static void init(void) {
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    bgmSlider = slider_create(250, 200, 300, 10, 0, 1, &BGM_VOLUME, "Assets/slider.png");
    sfxSlider = slider_create(250, 250, 300, 10, 0, 1, &SFX_VOLUME, "Assets/slider.png");
}

static void update(void) {

    update_button(&backButton);
    update_slider(&bgmSlider);
    update_slider(&sfxSlider);
    if (mouseState.buttons & 1 && backButton.hovered == true) {
        change_scene(create_menu_scene());
    }

}

static void draw(void) {
    // button
    draw_button(backButton, "BACK");
    draw_slider(&bgmSlider, al_map_rgb(128, 128, 128));
    draw_slider(&sfxSlider, al_map_rgb(128, 128, 128));

    al_draw_text(P3_FONT, al_map_rgb_f(1, 1, 1), 250, 175, 0, "BGM");
    al_draw_text(P3_FONT, al_map_rgb_f(1, 1, 1), 250, 225, 0, "SFX");
}

static void destroy(void) {
    destroy_button(&backButton);
    destroy_slider(&bgmSlider);
    destroy_slider(&sfxSlider);
}

Scene create_setting_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "setting";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
