#include <allegro5/allegro.h>
#include <string.h>
#include "utility.h"
#include "game.h"
#include "game_scene.h"
#include "UI.h"

Button nextLevelButton;
PlayerStat currentStat;
int level;
int points;

static void init(void) {
    nextLevelButton = button_create(SCREEN_W / 2 - 150, 650, 300, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
}

static void update(void) {
    update_button(&nextLevelButton);
    if (nextLevelButton.hovered && mouseState.buttons & 1) {
        change_scene(create_game_scene(level, currentStat));
    }
}

static void draw(void) {
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
