#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "loading_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"


static void init(void) {
}

static void update(void) {
}

static void draw(void) {
}

static void destroy(void) {
}


Scene create_level_change_scene() {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "level_change";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
