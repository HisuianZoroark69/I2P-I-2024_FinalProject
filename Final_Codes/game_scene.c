#include <string.h>
#include "game_scene.h"
#include "menu_scene.h"
#include "loading_scene.h"
#include "gameover_scene.h"
#include "game.h"
#include "player.h"
#include "map.h"
#include "enemy.h"
#include "weapon.h"
#include "UI.h"

#include <math.h>

Player player; // Player
Map map; // Map
enemyNode * enemyList; // Enemy List
BulletNode * bulletList; // Bullet List

// Weapon
Weapon weapon; 
int coins_obtained;

Point Camera;
const int CameraSoftBoundary = 64 * 5;

ALLEGRO_BITMAP* healthImg;

static void init(void){
    
    initEnemy();
    
    map = create_map("Assets/map0.txt", 0);

    player = create_player("Assets/arisu.png", "Assets/explode.png", 64, map.Spawn.x, map.Spawn.y);

    enemyList = createEnemyList();
    bulletList = createBulletList();

    weapon = create_weapon("Assets/guns.png", "Assets/yellow_bullet.png", 16, 8, 100);
    
    for(int i=0; i<map.EnemySpawnSize; i++){
        Enemy enemy = createEnemy(map.EnemySpawn[i].x, map.EnemySpawn[i].y, map.EnemyCode[i]);
        insertEnemyList(enemyList, enemy);
    }

    game_log("coord x:%d \n coords y:%d \n", map.Spawn.x, map.Spawn.y);
    healthImg = al_load_bitmap("Assets/heart.png");
    change_bgm("Assets/audio/game_bgm.mp3");
}

void UpdateCamera() {
    if (player.coord.x < Camera.x + CameraSoftBoundary)
        Camera.x = player.coord.x - CameraSoftBoundary;
    if(player.coord.x + TILE_SIZE > Camera.x + SCREEN_W - CameraSoftBoundary)
        Camera.x = player.coord.x - SCREEN_W + CameraSoftBoundary + TILE_SIZE;
    if (player.coord.y < Camera.y + CameraSoftBoundary)
        Camera.y = player.coord.y - CameraSoftBoundary;
    if (player.coord.y + TILE_SIZE > Camera.y + SCREEN_H - CameraSoftBoundary)
        Camera.y = player.coord.y - SCREEN_H + CameraSoftBoundary + TILE_SIZE;
}

static void update(void){
    /*
        [TODO Homework]
        
        Change the scene if winning/losing to win/lose scene
    */

    update_player(&player, &map);

    /*
        [TODO HACKATHON 1-3]
        
        Calcualte the formula for the Camera
        Camera.x = ...
        Camera.y = ...

        Hint: Adjust it based on player position variable, then subtract it with half of the gameplay screen
    */
    UpdateCamera();
    updateEnemyList(enemyList, &map, &player);
    if (player.status != PLAYER_ROOMBA) {
        update_weapon(&weapon, bulletList, player.coord, Camera);
    }
    updateBulletList(bulletList, enemyList, &map);
    update_map(&map, player.coord, &coins_obtained);
    
}

void drawHP() {
    if (player.health > 5) {
        al_draw_bitmap(healthImg, 0, 0, 0);
        al_draw_textf(P3_FONT, al_map_rgb(255, 255, 255), al_get_bitmap_width(healthImg) + 2, 2, 0, "x%d", player.health);
    }
    else {
        for (int i = 0; i < player.health; i++) {
            al_draw_bitmap(healthImg, i * al_get_bitmap_width(healthImg), 0, 0);
        }
    }
}

static void draw(void){
    /*
        [TODO HACKATHON 1-4]
        
        Calcualte the formula for the Camera
        Camera.x = ...
        Camera.y = ...

        Hint: Just copy from the [TODO HACKATHON 1-3]
    */
    
    // Draw
    draw_map(&map, Camera);
    drawEnemyList(enemyList, Camera);
    drawBulletList(bulletList, Camera);
    draw_player(&player, Camera);

    if (player.status != PLAYER_ROOMBA && player.status != PLAYER_DYING) {
        draw_weapon(&weapon, player.coord, Camera);
    }

    if (player.status == PLAYER_DYING && player.animation_tick >= 60) {
        change_scene(create_gameover_scene(al_get_backbuffer(al_get_current_display())));
    }
    /*
        [TODO Homework]
        
        Draw the UI of Health and Total Coins
    */
    drawHP();
}

static void destroy(void){
    delete_player(&player);
    delete_weapon(&weapon);
    destroy_map(&map);
    destroyBulletList(bulletList);
    destroyEnemyList(enemyList);
    terminateEnemy();
}


Scene create_game_scene(void){
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    
    scene.name = "game";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;
    
    return scene;
}
