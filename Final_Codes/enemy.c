#define _USE_MATH_DEFINES
#include "enemy.h"
#include "utility.h"

#include <math.h>
#include <stdlib.h>
#include "Item.h"

#define QUEUE_SIZE 2000

/*
    ENEMY IMPLEMENTATION
 */

ALLEGRO_BITMAP * slimeBitmap;
ALLEGRO_BITMAP* explode;
int MaxEnemySpawnCD = 60;
int EnemySpawnCD = 60;
int EnemyBaseHP;

// To check if p0 sprite and p1 sprite can go directly
static bool validLine(Map* map, Point p0, Point p1);
// To check if we can have a movement cutting directly to it
static bool bresenhamLine(Map * map, Point p0, Point p1);
// Find shortest path using BFS, could be better if you imeplement your own heap
static Point shortestPath(Map * map, Point src, Point dst);
// Calulate the movement speed to update and scaled it
static Point findScaledDistance(Point p1, Point p2);
static PointFloat findScaledDistanceF(Point p1, Point p2);
// Return true if enemy have collision with unwalkable tiles in map
static bool isCollision(Point enemyCoord, Map* map, enemyNode* dummyhead);
// Return true if player collide with enemy
static bool playerCollision(Point enemyCoord, Point playerCoord);


void initEnemy(void){
    // For memory efficiency, we load the image once
    // as every enemy bitmap are pointer, they will point to the same global bitmap variable

    // Slime
    char * slimePath = "Assets/Slime.png";
    slimeBitmap = al_load_bitmap(slimePath);
    explode = al_load_bitmap("Assets/explode.png");
    if(!slimeBitmap){
        game_abort("Error Load Bitmap with path : %s", slimePath);
    }
}

Enemy createEnemy(int row, int col, enemyType type){
    game_log("Creating Enemy at ( %d, %d )", row, col);
    
    Enemy enemy;
    memset(&enemy, 0, sizeof(enemy));
    
    enemy.animation_tick = 0;
    enemy.death_animation_tick = 0;
    enemy.status = ALIVE;
    
    enemy.coord = (Point){
        col * TILE_SIZE,
        row * TILE_SIZE
    };
    
    switch(type){
        case slime:
            enemy.health = EnemyBaseHP;
            enemy.type = slime;
            enemy.speed = 2;
            enemy.image = slimeBitmap;
            enemy.itemRate[Potion] = 5;
            enemy.itemRate[Coin] = 55;
            enemy.damage = 1;
            break;
        // Insert more here to have more enemy variant
        case c4slime:
            enemy.health = EnemyBaseHP >> 2; // 1/4 normal hp
            enemy.type = c4slime;
            enemy.speed = EnemyBaseHP / 10;  // Speed increase with hp
            enemy.image = slimeBitmap;
            enemy.itemRate[Potion] = 10;
            enemy.itemRate[Coin] = 60;
            enemy.damage = 5;
            break;
        default:
            enemy.health = EnemyBaseHP;
            enemy.type = slime;
            enemy.speed = 2;
            enemy.image = slimeBitmap;
            break;
    }
    
    return enemy;
}

// Return True if the enemy is dead
bool updateEnemy(Enemy* enemy, enemyNode* dummyhead, Map* map, Player* player) {
    
    if (enemy->status == DYING) {
        /*
            [TODO Homework]
            
            Configure the death animation tick for dying animation,
            Return true when the enemy is dead
        */ 
        enemy->death_animation_tick = (enemy->death_animation_tick + 1);
        if (enemy->death_animation_tick >= 8 && enemy->type == slime) return true;
        if (enemy->death_animation_tick >= 60) return true;
        return false;
    }
    
    enemy->animation_tick = (enemy->animation_tick + 1) % 64;

    if (enemy->animation_hit_tick > 0) {
        enemy->animation_tick = (enemy->animation_tick + 1) % 64;
        enemy->animation_hit_tick--;
    }
    
    //Only handle knockbacks, no damage calculation
    if (enemy->knockback_CD > 0) {
        enemy->knockback_CD--;
        int next_x = enemy->coord.x + 4 * cos(enemy->knockback_angle);
        int next_y = enemy->coord.y + 4 * sin(enemy->knockback_angle);
        Point next;
        next = (Point){ next_x, enemy->coord.y };
        
        if (!isCollision(next, map, dummyhead)) {
            enemy->coord = next;
        }
        
        next = (Point){ enemy->coord.x, next_y };
        if (!isCollision(next, map, dummyhead)) {
            enemy->coord = next;
        }
    }
    else {
        /*
            [TODO Homework]

            Replace delta variable with the function below to start enemy movement
            Point delta = shortestPath(map, enemy->coord, player->coord);
        */

        //Point delta = shortestPath(map, enemy->coord, player->coord);
        PointFloat delta = findScaledDistanceF(enemy->coord, player->coord);
        Point next, prev = enemy->coord;

        if (delta.x > 0) enemy->dir = RIGHT;
        if (delta.x < 0) enemy->dir = LEFT;

        next = (Point){ round((float)enemy->coord.x + delta.x * (float)enemy->speed), enemy->coord.y };
        if (!isCollision(next, map, dummyhead))
            enemy->coord = next;

        next = (Point){ enemy->coord.x, round((float)enemy->coord.y + delta.y * (float)enemy->speed) };
        if (!isCollision(next, map, dummyhead))
            enemy->coord = next;

        // To fix bug if the enemy need to move a little bit, the speed will not be use
        if (enemy->coord.x == prev.x && enemy->coord.y == prev.y) {
            next = (Point){ enemy->coord.x + delta.x, enemy->coord.y };
            if (!isCollision(next, map, dummyhead))
                enemy->coord = next;

            next = (Point){ enemy->coord.x, enemy->coord.y + delta.y };
            if (!isCollision(next, map, dummyhead))
                enemy->coord = next;
        }

        //Update enemy
        if (playerCollision(enemy->coord, player->coord) && enemy->animation_hit_tick == 0) {
            //KMS if player is in roomba mode, dealing 0 damage
            if (player->status == PLAYER_ROOMBA) {
                enemy->status = DYING;
                enemy->health = 0;
                return false;
            }
            hitPlayer(player, enemy->coord, enemy->damage);
            if (enemy->type == slime) {
                enemy->animation_tick = 0;
                enemy->animation_hit_tick = 32;
            }
            if (enemy->type == c4slime) {
                enemy->health = 0;
                enemy->status = DYING;
            }
        }
    }
    return false;
}

void drawEnemy(Enemy * enemy, Point cam){

    int dy = enemy->coord.y - cam.y; // destiny y axis
    int dx = enemy->coord.x - cam.x; // destiny x axis

    if(enemy->status == ALIVE){
        int offset = 16 * (int)(enemy->animation_tick / 8);
        if(enemy->animation_hit_tick > 0){
            offset += 16 * 8;
        }
        int flag = enemy->dir == RIGHT ? 1 : 0;
        int tint_red = enemy->knockback_CD > 0 ? 255 : 0;
        
        if (enemy->type == slime) {
            al_draw_tinted_scaled_rotated_bitmap_region(enemy->image, offset, 0, 16, 16, al_map_rgb(tint_red, 255, 255),
                0, 0, dx, dy, TILE_SIZE / 16, TILE_SIZE / 16,
                0, flag);
        }
        if (enemy->type == c4slime) {
            al_draw_tinted_scaled_rotated_bitmap_region(enemy->image, offset, 0, 16, 16, al_map_rgb(tint_red, 0, 255),
                0, 0, dx, dy, TILE_SIZE / 16, TILE_SIZE / 16,
                0, flag);
        }
    }
    else if(enemy->status == DYING){
        /*
            [TODO Homework]

            Draw Dying Animation for enemy
        */
        ALLEGRO_BITMAP* img = enemy->type == slime ? enemy->image : explode;
        int frameSize = enemy->type == slime ? 16 : 64;
        int y = enemy->type == slime ? 16 : 0;
        int offset = frameSize * (int)(enemy->death_animation_tick);
        int flag = enemy->dir == RIGHT ? 1 : 0;
        al_draw_tinted_scaled_rotated_bitmap_region(img, offset, y, frameSize, frameSize, al_map_rgb(255, 255, 255),
            0, 0, dx, dy, TILE_SIZE / frameSize, TILE_SIZE / frameSize,
            0, flag);
    }
    
#ifdef DRAW_HITBOX
    al_draw_rectangle(
        dx, dy, dx + TILE_SIZE, dy + TILE_SIZE,
        al_map_rgb(255, 0, 0), 1
    );
#endif
}

void destroyEnemy(Enemy * enemy){

}

void terminateEnemy(void) {
    al_destroy_bitmap(slimeBitmap);
    al_destroy_bitmap(explode);
}

void hitEnemyNoKnockback(Enemy* enemy, int damage) {
    enemy->health -= damage;
    if (enemy->health <= 0) {
        enemy->health = 0;
        enemy->status = DYING;
    }
}

void hitEnemy(Enemy * enemy, int damage, float angle){
    hitEnemyNoKnockback(enemy, damage);
    enemy->knockback_angle = angle;
    enemy->knockback_CD = 16;
}

/*
    LINKED LIST IMPLEMENTATION FOR ENEMY
    THERE ARE :
        (1) CREATE      : INITIALIZING DUMMY HEAD
        (2) INSERT      : INSERT NEW NODE OF ENEMY
        (3) UPDATE      : ITERATE EVERYTHING AND UPDATE THE ENEMY
        (4) DRAW        : ITERATE EVERYTHING AND DRAW THE ENEMY
        (5) DESTROY     : DESTROY THE LINKED LIST
 */

enemyNode* createEnemyList(int baseHP) {
    enemyNode * dummyhead = (enemyNode *) malloc(sizeof(enemyNode));
    dummyhead->next = NULL;
    EnemyBaseHP = baseHP;
    return dummyhead;
}

void insertEnemyList(enemyNode * dummyhead, Enemy _enemy){
    enemyNode * tmp = (enemyNode *) malloc(sizeof(enemyNode));
    tmp->enemy = _enemy;
    tmp->next = dummyhead->next;
    dummyhead->next = tmp;
}

void updateEnemyList(enemyNode* dummyhead, Map* map, Player* player, ItemNode* itemList) {
    enemyNode* cur = dummyhead->next;
    enemyNode* prev = dummyhead;

    EnemySpawnCD--;
    if (EnemySpawnCD <= 0) {
        EnemySpawnCD = MaxEnemySpawnCD;
        int nx, ny;
        do {
            nx = RandNum(0, map->col);
            ny = RandNum(0, map->row);
        } while (!isWalkable(map, (Point) { nx, ny }) && !isCollision((Point) { nx, ny }, map, dummyhead));
        enemyType type = (RandNum(0, 100) < 10) ? c4slime : slime;
        insertEnemyList(dummyhead, createEnemy(nx, ny, type));
    }
    
    while(cur != NULL){
        bool shouldDelete = updateEnemy(&cur->enemy, dummyhead, map, player);
        if(shouldDelete){
            //Spawn item, the item spawned is the one the rate is smaller but closest to
            int rate = RandNum(0, 100);
            int distance = 100;
            ItemType itemType = ItemTypeCount;
            for (int i = 0; i < ItemTypeCount; i++) {
                int curItemRate = cur->enemy.itemRate[i];
                int comp = curItemRate - rate;
                if (rate < curItemRate && comp < distance && comp >= 0) {
                    itemType = i;
                    distance = rate - curItemRate;
                }
            }
            game_log("Current rate: %2d, distance: %2d, item type: %d", rate, distance, itemType);
            insert_item_list(itemList, create_item(itemType, cur->enemy.coord));

            //Delete the enemy
            prev->next = cur->next;
            destroyEnemy(&cur->enemy);
            free(cur);
            cur = prev->next;

        }
        else{
            prev = cur;
            cur = cur->next;
        }
    }
}

void drawEnemyList(enemyNode * dummyhead, Point cam){
    enemyNode * cur = dummyhead->next;
    
    while(cur != NULL){
        drawEnemy(&cur->enemy, cam);
        cur = cur->next;
    }
}

void destroyEnemyList(enemyNode * dummyhead){
    while(dummyhead != NULL){
        enemyNode * del = dummyhead;
        dummyhead = dummyhead->next;
        destroyEnemy(&del->enemy);
        free(del);
    }
}

static bool validLine(Map * map, Point p0, Point p1){
    int offsetX[4] = {0, 0, TILE_SIZE-1, TILE_SIZE-1};
    int offsetY[4] = {0, TILE_SIZE-1, 0, TILE_SIZE-1};
    
    for(int i=0; i<4; i++){
        Point n1 = (Point){
            p0.x + offsetX[i],
            p0.y + offsetY[i]
        };
        
        Point n2 = (Point){
            p1.x + offsetX[i],
            p1.y + offsetY[i]
        };
        if(bresenhamLine(map, n1, n2)) return false;
    }
    
    return true;
}

static Point shortestPath(Map * map, Point enemy, Point player){
    // Point enemy & player is pixel coordinate
    static DIRECTION dir[MAX_MAP_ROW][MAX_MAP_COL]; // to backtrack from dst to src
    static bool visit[MAX_MAP_ROW][MAX_MAP_COL];
    memset(visit, 0, sizeof(visit));
    
    // Point declared below is not coordinate of pixel, but COORDINATE OF ARRAY!
    Point src = (Point){
        enemy.y / TILE_SIZE,
        enemy.x / TILE_SIZE
    };
    Point dst = (Point){
        player.y / TILE_SIZE,
        player.x / TILE_SIZE
    };
    
    static Point Queue[QUEUE_SIZE];
    int front = 0, rear = 0;
    
    Queue[rear++] = src;
    bool found = false;
    
    // Movement set
    static int dx[4] = {1, -1, 0, 0};
    static int dy[4] = {0, 0, 1, -1};
    static DIRECTION move[4] = {UP, DOWN, LEFT, RIGHT}; // To backtrack
    
    while(front != rear){
        Point cur = Queue[front++];
        
        // Found the destiny
        if(cur.x == dst.x && cur.y == dst.y) {
            found = true;
            break;
        };
        
        for(int i=0; i<4; i++){
            Point next = (Point){
                cur.x + dx[i],
                cur.y + dy[i]
            };
            
            if(next.x < 0 || next.y < 0) continue;
            if(next.x >= map->row || next.y >= map->col) continue;
            
            if(isWalkable(map, next) && !visit[next.x][next.y]){
                dir[next.x][next.y] = move[i];
                visit[next.x][next.y] = true;
                Queue[rear++] = next;
            }
        }
    }
    
    // Toward a String-Pulling Approach to Path Smoothing on Grid Graphs
    // http://idm-lab.org/bib/abstracts/papers/socs20c.pdf
    if(found){
        if(validLine(map, enemy, player))
            return findScaledDistance(enemy, player);
        
        int max_iteration = 1000;
        Point it = dst;
        while(max_iteration--){
            Point translate_it = (Point){
                it.y * TILE_SIZE,
                it.x * TILE_SIZE
            };
            
            if(validLine(map, enemy, translate_it))
                return findScaledDistance(enemy, translate_it);
            
            switch(dir[it.x][it.y]){
                case UP:
                    it.x--;
                    break;
                case DOWN:
                    it.x++;
                    break;
                case LEFT:
                    it.y--;
                    break;
                case RIGHT:
                    it.y++;
                    break;
                default:
                    goto END;
            }
            
        }
        END:
        game_log("FAILED TO ITERATE");
    }
    return (Point){0, 0};
}

static bool bresenhamLine(Map * map, Point p0, Point p1){
    int dx =  abs(p1.x - p0.x), sx = p0.x < p1.x ? 1 : -1;
    int dy =  -abs(p1.y - p0.y), sy = p0.y < p1.y ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        int tile_x = p0.y / TILE_SIZE;
        int tile_y = p0.x / TILE_SIZE;
        if(!isWalkable(map, (Point){tile_x, tile_y})) return true;
        
        if (p0.x == p1.x && p0.y == p1.y) break;
        e2 = 2 * err;

        if (e2 > dy) {
            err += dy;
            p0.x += sx;
        } else if (e2 < dx) {
            err += dx;
            p0.y += sy;
        }
    }
    
    return false;
}

static Point findScaledDistance(Point p1, Point p2) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    
    double d = sqrt(dx * dx + dy * dy);
    
    // Floating error fix, when smaller than delta it will be immediately 0
    if (d < 0.001) {
        return (Point) {0, 0};
    }

    double dxUnit = dx / d;
    double dyUnit = dy / d;
    
    return (Point){round(dxUnit), round(dyUnit)};
}

static PointFloat findScaledDistanceF(Point p1, Point p2) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;

    double d = sqrt(dx * dx + dy * dy);

    // Floating error fix, when smaller than delta it will be immediately 0
    if (d < 0.001) {
        return (PointFloat) { 0, 0 };
    }

    double dxUnit = dx / d;
    double dyUnit = dy / d;

    return (PointFloat) { dxUnit, dyUnit};
}

static bool playerCollision(Point enemyCoord, Point playerCoord){
    // Rectangle & Rectanlge Collision
    if (enemyCoord.x < playerCoord.x + TILE_SIZE &&
        enemyCoord.x + TILE_SIZE > playerCoord.x &&
        enemyCoord.y < playerCoord.y + TILE_SIZE &&
        enemyCoord.y + TILE_SIZE > playerCoord.y) {
            return true;
    } else {
        return false;
    }
}

//This isCollision shall check for collision with other slimes, avoiding overlap
static bool isCollision(Point enemyCoord, Map* map, enemyNode* dummyhead) {
    if (enemyCoord.x < 0 ||
        enemyCoord.y < 0 ||
        (enemyCoord.x + TILE_SIZE - 1) / TILE_SIZE >= map->col ||
        (enemyCoord.y + TILE_SIZE - 1) / TILE_SIZE >= map->row)
        return true;

    //If the distance is larger than 1, its not the current one
    enemyNode* cur = dummyhead->next;
    //while (cur != NULL) {
    //    //Use AABB
    // 
    //    if (enemyCoord.x < cur->enemy.coord.x + TILE_SIZE &&
    //        enemyCoord.x + TILE_SIZE > cur->enemy.coord.x &&
    //        enemyCoord.y < cur->enemy.coord.y + TILE_SIZE &&
    //        enemyCoord.y + TILE_SIZE > cur->enemy.coord.y) {
    //        //Collision detected, check if checking current square
    //        double distanceSquared = (enemyCoord.x - cur->enemy.coord.x) * (enemyCoord.x - cur->enemy.coord.x) +
    //                                 (enemyCoord.y - cur->enemy.coord.y) * (enemyCoord.y - cur->enemy.coord.y);
    //        if (distanceSquared > 1) return true;
    //    }
    //    cur = cur->next;
    //}
    

    return false;
}
