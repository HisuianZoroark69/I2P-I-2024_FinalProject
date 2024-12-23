#include <allegro5/allegro.h>
#include <string.h>
#include "utility.h"
#include "game.h"
#include "game_scene.h"
#include "UI.h"
#define statCount 5

static ALLEGRO_FONT* font;
Button nextLevelButton;
Button addStatButtons[statCount];

PlayerStat currentStat;
int level;
int points;

static void init(void) {
    nextLevelButton = button_create(SCREEN_W / 2 - 150, 650, 300, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    for (int i = 0; i < statCount; i++) {
        addStatButtons[i] = button_create(500, 50 + 100 * i, 100, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    }
    font = al_load_font("Assets/YouDiedFont.otf", 40, 0);

    //Level up prize
    points += 3;
    currentStat.health += 3;
}

static void update(void) {
    update_button(&nextLevelButton);
    for (int i = 0; i < statCount; i++) {
        update_button(&addStatButtons[i]);
    }
    if (nextLevelButton.hovered && mouseButtonUp & 1) {
        change_scene(create_game_scene(level, points, currentStat));
    }

    //Stat increase
    if (points <= 0) return;
    if (addStatButtons[0].hovered && mouseButtonUp & 1) {
        currentStat.health += 1;
        points--;
    }
    if (addStatButtons[1].hovered && mouseButtonUp & 1) {
        currentStat.atk += 2;
        points--;
    }
    if (addStatButtons[2].hovered && mouseButtonUp & 1 && currentStat.atkSpd >= 2) {
        currentStat.atkSpd -= 1;
        points--;
    }
    if (addStatButtons[3].hovered && mouseButtonUp & 1 && currentStat.speed <= 10) {
        currentStat.speed *= 1.15;
        if (currentStat.speed >= 10) currentStat.speed = 10;
        points--;
    }
    if (addStatButtons[4].hovered && mouseButtonUp & 1 && currentStat.evasion <= 50) {
        currentStat.evasion += 5;
        points--;
    }
}

static void draw(void) {
    al_draw_textf(font, al_map_rgb_f(1, 1, 0), 1, 0, 0, "Coins: %d", points);
    al_draw_textf(font, al_map_rgb(255, 255, 255), 100, 100, 0, "HP: %d", currentStat.health);
    al_draw_textf(font, al_map_rgb(255, 255, 255), 100, 200, 0, "Atk: %.0f", currentStat.atk);
    al_draw_textf(font, al_map_rgb(255, 255, 255), 100, 300, 0, "AtkSpd: %.0f", currentStat.atkSpd);
    al_draw_textf(font, al_map_rgb(255, 255, 255), 100, 400, 0, "Speed: %.1f", currentStat.speed);
    al_draw_textf(font, al_map_rgb(255, 255, 255), 100, 500, 0, "Evasion: %.0f", currentStat.evasion);


    for (int i = 0; i < statCount; i++) {
        draw_button(addStatButtons[i], "+");
    }
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
