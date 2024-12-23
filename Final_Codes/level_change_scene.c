#include <allegro5/allegro.h>
#include <string.h>
#include "utility.h"
#include "game.h"
#include "game_scene.h"
#include "UI.h"

static ALLEGRO_FONT* font;
Button nextLevelButton;
Button addHPButton;
PlayerStat currentStat;
int level;
int points;

static void init(void) {
    nextLevelButton = button_create(SCREEN_W / 2 - 150, 650, 300, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    addHPButton = button_create(500, 50, 100, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    font = al_load_font("Assets/YouDiedFont.otf", 50, 0);
    points += 3;
}

static void update(void) {
    update_button(&nextLevelButton);
    update_button(&addHPButton);
    if (nextLevelButton.hovered && mouseButtonUp & 1) {
        change_scene(create_game_scene(level, points, currentStat));
    }
    if (addHPButton.hovered && mouseButtonUp & 1) {
        currentStat.health += 1;
        points--;
    }
}

static void draw(void) {
    al_draw_textf(font, al_map_rgb_f(1, 1, 0), 1, 0, 0, "Coins: %d", points);
    al_draw_textf(font, al_map_rgb(255, 255, 255), 100, 100, 0, "HP: %d", currentStat.health);
    draw_button(addHPButton, "+");
    draw_button(nextLevelButton, "Next level");
}

static void destroy(void) {
    destroy_button(&nextLevelButton);
}


Scene create_level_change_scene(int nextLevel, int upgradePoints, PlayerStat stat) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "level_change";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;
    currentStat = stat;
    level = nextLevel;
    points = upgradePoints;
    return scene;
}
