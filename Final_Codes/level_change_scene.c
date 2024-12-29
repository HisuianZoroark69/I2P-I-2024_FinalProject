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
int prices[statCount];

PlayerStat currentStat;
extern PlayerStat defaultStat;
int level;
int points;

static void init(void) {
    nextLevelButton = button_create(SCREEN_W / 2 - 150, 650, 300, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    for (int i = 0; i < statCount; i++) {
        addStatButtons[i] = button_create(500, 50 + 100 * i, 150, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    }
    font = al_load_font("Assets/YouDiedFont.otf", 40, 0);

    //Level up prize
    points += 1;
    change_bgm("Assets/audio/game_music2.mp3", true, AUDIO_FADE_TIME);
    //currentStat.health += 3;
}

static void update(void) {
    update_button(&nextLevelButton);
    for (int i = 0; i < statCount; i++) {
        if(points >= prices[i])
            update_button(&addStatButtons[i]);
    }
    if (nextLevelButton.hovered && mouseButtonUp & 1) {
        change_scene(create_game_scene(level, points, currentStat), SCENE_FADE_TIME);
    }
    //update prices
    prices[0] = 1;
    prices[1] = (currentStat.atk - defaultStat.atk + 2) / 2;
    prices[2] = defaultStat.atkSpd - currentStat.atkSpd + 1;
    prices[3] = currentStat.speed - defaultStat.speed + 1;
    prices[4] = currentStat.evasion - defaultStat.evasion + 5;

    //Stat increase
    if (addStatButtons[0].hovered && mouseButtonUp & 1 && points >= prices[0]) {
        currentStat.health += 1;
        points -= prices[0];
    }
    if (addStatButtons[1].hovered && mouseButtonUp & 1 && points >= prices[1]) {
        currentStat.atk += 2;
        points -= prices[1];
    }
    if (addStatButtons[2].hovered && mouseButtonUp & 1 && currentStat.atkSpd > 2 && points >= prices[2]) {
        currentStat.atkSpd -= 1;
        points -= prices[2];
    }
    if (addStatButtons[3].hovered && mouseButtonUp & 1 && currentStat.speed < 10 && points >= prices[3]) {
        currentStat.speed += 1;
        if (currentStat.speed >= 10) currentStat.speed = 10;
        points -= prices[3];
    }
    if (addStatButtons[4].hovered && mouseButtonUp & 1 && currentStat.evasion < 50 && points >= prices[4]) {
        currentStat.evasion += 5;
        points -= prices[4];
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
        if (i == 2 && currentStat.atkSpd <= 2) continue;
        if (i == 3 && currentStat.speed >= 10) continue;
        if (i == 4 && currentStat.evasion >= 50) continue;
        if (points < prices[i]) continue;
        char buffer[10];
        sprintf(buffer, "%d", prices[i]);
        draw_button(addStatButtons[i], buffer);
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
