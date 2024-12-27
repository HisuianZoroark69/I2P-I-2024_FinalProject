#include <string.h>
#include "game_scene.h"
#include "menu_scene.h"
#include "gameover_scene.h"
#include "level_change_scene.h"
#include "game.h"
#include "player.h"
#include "map.h"
#include "enemy.h"
#include "weapon.h"
#include "UI.h"
#include "Item.h"

#include <math.h>
#define Grace_Period 3

Player player; // Player
PlayerStat pStat;
Map map; // Map
enemyNode * enemyList; // Enemy List
BulletNode * bulletList; // Bullet List
ItemNode* itemList;

// Weapon
Weapon weapon;

Point Camera;
const int CameraSoftBoundary = 64 * 5;

ALLEGRO_BITMAP* healthImg;
ALLEGRO_BITMAP* coinIconImg;

int currentLevel, timeLimit;
int upgradePoints;

static void init(void){
    
    initEnemy();
    init_item();
    
    map = create_map("Assets/map0.txt", 0);

    player = create_player("Assets/arisu.png", "Assets/explode.png", 64, map.Spawn.x, map.Spawn.y, pStat);

    enemyList = createEnemyList(currentLevel * 10);
    bulletList = createBulletList();
    itemList = create_item_list();

    weapon = create_weapon("Assets/guns.png", "Assets/yellow_bullet.png", pStat.atkSpd, 8, pStat.atk);
    
    for(int i=0; i<map.EnemySpawnSize; i++){
        Enemy enemy = createEnemy(map.EnemySpawn[i].x, map.EnemySpawn[i].y, map.EnemyCode[i]);
        insertEnemyList(enemyList, enemy);
    }

    game_log("coord x:%d \n coords y:%d \n", map.Spawn.x, map.Spawn.y);
    healthImg = al_load_bitmap("Assets/heart.png");
    coinIconImg = al_load_bitmap("Assets/coin_icon.png");
    change_bgm("Assets/audio/game_music2.mp3", true);
    
    timeLimit = (10 + currentLevel * (5) + Grace_Period) * FPS; //3 seconds grace period to pick up coins
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
    //Update timer
    if(player.status != PLAYER_DYING) timeLimit--;
    //Grace period reached
    if (timeLimit == Grace_Period * FPS) {
        //destroyBulletList(bulletList);
        killAllEnemyList(enemyList);
    }
    if (timeLimit <= 0) {
        change_scene(create_level_change_scene(currentLevel + 1, upgradePoints, player.stat));
        return;
    }

    update_player(&player, &map, weapon.cooldown_counter + mouseState.buttons);
    update_item_list(itemList, (ItemParam){&player, &upgradePoints});
    UpdateCamera();
    if (player.status != PLAYER_ROOMBA && player.status != PLAYER_TRANSFORMING && timeLimit > Grace_Period * FPS) {
        update_weapon(&weapon, bulletList, player.coord, Camera);
    }
    updateEnemyList(enemyList, &map, &player, itemList);
    updateBulletList(bulletList, enemyList, &map);
    update_map(&map, player.coord, &upgradePoints);
    
}

void drawUI() {
    if (player.stat.health > 5) {
        al_draw_bitmap(healthImg, 0, 0, 0);
        al_draw_textf(P3_FONT, al_map_rgb(255, 255, 255), al_get_bitmap_width(healthImg) + 2, 2, 0, "x%d", player.stat.health);
    }
    else {
        for (int i = 0; i < player.stat.health; i++) {
            al_draw_bitmap(healthImg, i * al_get_bitmap_width(healthImg), 0, 0);
        }
    }
    //UI
    al_draw_bitmap(coinIconImg, 0, 30, 0);
    al_draw_textf(P3_FONT, al_map_rgb(255, 255, 255), al_get_bitmap_width(coinIconImg), 30, 0, "%d", upgradePoints);
}

void drawTimeLimit() {
    al_draw_textf(P3_FONT, al_map_rgb(255, 255, 255), 400, 20, 0, "Level: %d", currentLevel);
    al_draw_textf(P3_FONT, al_map_rgb(255, 255, 255), 400, 50, 0, "Time: %d", (timeLimit - Grace_Period) / FPS);
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

    if (player.status != PLAYER_ROOMBA && player.status != PLAYER_TRANSFORMING && player.status != PLAYER_DYING) {
        draw_weapon(&weapon, player.coord, Camera);
    }
    draw_item_list(itemList, &Camera);

    /*
        [TODO Homework]
        
        Draw the UI of Health and Total Coins
    */
    drawUI();
    drawTimeLimit();
    if (player.status == PLAYER_DYING && player.animation_tick >= 60) {
        change_scene(create_gameover_scene(al_get_backbuffer(al_get_current_display()), currentLevel));
    }
}

static void destroy(void){
    al_destroy_bitmap(healthImg);
    al_destroy_bitmap(coinIconImg);
    delete_player(&player);
    delete_weapon(&weapon);
    destroy_map(&map);
    destroyEnemyList(enemyList);
    destroyBulletList(bulletList);
    destroy_item_list(itemList);
    terminateEnemy();
    terminate_item();
}


Scene create_game_scene(int level, int points, PlayerStat stat) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    
    scene.name = "game";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;
    
    currentLevel = level;
    pStat = stat;
    upgradePoints = points;

    return scene;
}
