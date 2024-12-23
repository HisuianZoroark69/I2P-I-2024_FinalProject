#include "player.h"
#include "game.h"
#include "utility.h"
#include "map.h"

#include <math.h>

static int isCollision(Player* player, Map* map);
int PlayerFrameSize;

Player create_player(char * path, char* death, int frameSize, int row, int col, PlayerStat stat){
    Player player;
    memset(&player, 0, sizeof(player));
    PlayerFrameSize = frameSize;
    player.coord = (Point){
        col * TILE_SIZE,
        row * TILE_SIZE
    };
    player.stat = stat;

    player.image = al_load_bitmap(path);
    if(!player.image){
        game_abort("Error Load Bitmap with path : %s", path);
    }
    player.death = al_load_bitmap(death);
    if (!player.death) {
        game_abort("Error Load Bitmap with path %s", death);
    }

    return player;
}

void change_status(Player* player, PLAYER_STATUS status) {
    if (player->status == status) return;
    player->animation_tick = 0;
    player->status = status;
}
void update_player(Player * player, Map* map, int isWeaponShooting){

    Point original = player->coord;

    if (player->status == PLAYER_DYING) {
        player->animation_tick++;
        if (player->animation_tick == 59) {
            return 1;
        }
        return 0;
    }
    if (player->stat.health <= 0) {
        player->stat.health = 0;
        change_status(player, PLAYER_DYING);
        return;
    }

    // Knockback effect, now damage indicator
    if(player->knockback_CD > 0){
        player->knockback_CD--;
    }

    //Stop player moving in direction if not walkable
    double dirX = 0, dirY = 0; 

    if(keyState[ALLEGRO_KEY_W]) dirY -= 1;
    if(keyState[ALLEGRO_KEY_S]) dirY += 1;
    if(keyState[ALLEGRO_KEY_A]) dirX -= 1;
    if(keyState[ALLEGRO_KEY_D]) dirX += 1;

    if(dirX || dirY){
        player->direction = atan2(dirY, dirX);
        player->coord.x = round((float)player->coord.x + (float)player->stat.speed * cos(player->direction));
        player->coord.y = round((float)player->coord.y + (float)player->stat.speed * sin(player->direction));
    }
    if (isWeaponShooting) {
        change_status(player, PLAYER_SHOOTING);
    }
    else if(dirX || dirY){
        change_status(player, PLAYER_WALKING);
    }
    else {
        change_status(player, PLAYER_IDLE);
    }
    DIRECTION collisionDir = isCollision(player, map);
    if (collisionDir & UP || collisionDir & DOWN) player->coord.y = original.y;
    if (collisionDir & LEFT || collisionDir & RIGHT) player->coord.x = original.x;

    player->animation_tick = (player->animation_tick + 1) % 60;
    return 0;
}

void draw_player(Player * player, Point cam){
    int dy = player->coord.y - cam.y; // destiny y axis
    int dx = player->coord.x - cam.x; // destiny x axis

#ifdef DRAW_HITBOX
    al_draw_rectangle(
        dx, dy, dx + TILE_SIZE, dy + TILE_SIZE,
        al_map_rgb(255, 0, 0), 1
    );
#endif

    if (-PI / 2 < player->direction && player->direction < PI / 2) player->flip = 0;
    else if (player->direction == -PI / 2 || player->direction == PI / 2); //Dont change direction if going up or down
    else player->flip = ALLEGRO_FLIP_HORIZONTAL;

    ALLEGRO_BITMAP* image;
    if (player->status == PLAYER_DYING) image = player->death;
    else image = player->image;

    int xOffset = player->animation_tick;
    if (player->status != PLAYER_DYING) {
        xOffset %= 32;
        xOffset /= 4;
    }
    if (player->status == PLAYER_WALKING) xOffset = 0;
    xOffset *= PlayerFrameSize;

    int yOffset;
    switch (player->status) {
    case PLAYER_DYING:
    case PLAYER_IDLE: yOffset = 0; break;
    case PLAYER_WALKING: yOffset = PlayerFrameSize; break;
    case PLAYER_SHOOTING: yOffset = PlayerFrameSize * 2; break;
    }

    if (player->status == PLAYER_WALKING) {
        if (player->animation_tick % 15 > 7) dy -= TILE_SIZE / 8;
    }

    al_draw_tinted_scaled_bitmap(image, al_map_rgb(255, 255, 255),
        xOffset, yOffset, PlayerFrameSize, PlayerFrameSize, // source image x, y, width, height
        dx, dy, TILE_SIZE, TILE_SIZE, // destination x, y, width, height
        player->flip // Flip or not
    );



}

void delete_player(Player * player){
    al_destroy_bitmap(player->image);
}


static int isCollision(Player* player, Map* map){
    Point corners[4] = {
        {player->coord.x / TILE_SIZE, player->coord.y / TILE_SIZE},                                     //TL
        {(player->coord.x + TILE_SIZE - 1) / TILE_SIZE, player->coord.y / TILE_SIZE},                   //TR
        {player->coord.x / TILE_SIZE, (player->coord.y + TILE_SIZE - 1) / TILE_SIZE},                   //BL
        {(player->coord.x + TILE_SIZE - 1) / TILE_SIZE, (player->coord.y + TILE_SIZE - 1) / TILE_SIZE}  //BR
    };
 
    int ret = NONE;
    if(!isWalkable(map, corners[0]) && !isWalkable(map, corners[1])) ret ^= UP;
    if(!isWalkable(map, corners[2]) && !isWalkable(map, corners[3])) ret ^= DOWN;
    if(!isWalkable(map, corners[0]) && !isWalkable(map, corners[2])) ret ^= LEFT;
    if(!isWalkable(map, corners[1]) && !isWalkable(map, corners[3])) ret ^= RIGHT;

    return ret;
}

void hitPlayer(Player * player, Point enemy_coord, int damage){
    if(player->knockback_CD == 0){
        float dY = player->coord.y - enemy_coord.y;
        float dX = player->coord.x - enemy_coord.x;
        float angle = atan2(dY, dX);

        player->knockback_angle = angle;
        player->knockback_CD = 1;

        player->stat.health -= 1;
    }
}
