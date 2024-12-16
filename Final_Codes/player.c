#include "player.h"
#include "game.h"
#include "utility.h"
#include "map.h"

#include <math.h>

static int isCollision(Player* player, Map* map);

Player create_player(char * path, int row, int col){
    Player player;
    memset(&player, 0, sizeof(player));

    player.coord = (Point){
        col * TILE_SIZE,
        row * TILE_SIZE
    };

    player.speed = 4;
    player.health = 50;

    player.image = al_load_bitmap(path);
    if(!player.image){
        game_abort("Error Load Bitmap with path : %s", path);
    }

    return player;
}

void update_player(Player * player, Map* map){

    Point original = player->coord;

    // Knockback effect
    if(player->knockback_CD > 0){

        player->knockback_CD--;
        
    }

    /*
        [TODO HACKATHON 1-1]

        Player Movement
        Adjust the movement by player->speed

        if (keyState[ALLEGRO_KEY_W]) {
            player->coord.y = ...
            player->direction = ...
        }
        if (keyState[ALLEGRO_KEY_S]) {
            player->coord.y = ...
            player->direction = ...
        }
    */

    // if Collide, snap to the grid to make it pixel perfect
    // if(isCollision(player, map)){
    //     player->coord.y = round((float)original.y / (float)TILE_SIZE) * TILE_SIZE;
    // }

    /*
        [TODO HACKATHON 1-2]

        Player Movement
        Add/Subtract the movement by player->speed

        if (keyState[ALLEGRO_KEY_A]) {
            player->coord.y = ...
            player->direction = ...
        }
        if (keyState[ALLEGRO_KEY_D]) {
            player->coord.y = ...
            player->direction = ...

    }
    */
    //Stop player moving in direction if not walkable
    double dirX = 0, dirY = 0; 

    if(keyState[ALLEGRO_KEY_W]) dirY -= 1;
    if(keyState[ALLEGRO_KEY_S]) dirY += 1;
    if(keyState[ALLEGRO_KEY_A]) dirX -= 1;
    if(keyState[ALLEGRO_KEY_D]) dirX += 1;

    if(dirX || dirY){
        player->direction = atan2(dirY, dirX);
        player->coord.x = round((float)player->coord.x + (float)player->speed * cos(player->direction));
        player->coord.y = round((float)player->coord.y + (float)player->speed * sin(player->direction));
        //game_log("Player move with direction %d", (int)(player->direction * 180 / PI));
    }
    DIRECTION collisionDir = isCollision(player, map);
    if (collisionDir & UP || collisionDir & DOWN) player->coord.y = original.y;
    if (collisionDir & LEFT || collisionDir & RIGHT) player->coord.x = original.x;

    // if(isCollision(player, map)){
    //     player->coord.x = round((float)original.x / (float)TILE_SIZE) * TILE_SIZE;
    // }

    /*
        [TODO Homework]

        Calculate the animation tick to draw animation later
    */
    player->animation_tick = (player->animation_tick + 1) % 64;
}

void draw_player(Player * player, Point cam){
    int dy = player->coord.y - cam.y; // destiny y axis
    int dx = player->coord.x - cam.x; // destiny x axis

    int flag = 0; // Change the flag to flip character

    /*
        [TODO Homework]

        Draw Animation of Dying, Walking, and Idle
    */

    al_draw_tinted_scaled_bitmap(player->image, al_map_rgb(255, 255, 255),
        0, 0, 32, 32, // source image x, y, width, height
        dx, dy, TILE_SIZE, TILE_SIZE, // destination x, y, width, height
        flag // Flip or not
    );


#ifdef DRAW_HITBOX
    al_draw_rectangle(
        dx, dy, dx + TILE_SIZE, dy + TILE_SIZE,
        al_map_rgb(255, 0, 0), 1
    );
#endif

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

        /*
            [TODO Homework]

            Game Logic when the player get hit or die
        */

        player->knockback_angle = angle;
        player->knockback_CD = 1;

        player->health -= 1;
        if(player->health <= 0){
            player->health = 0;
            player->status = PLAYER_DYING;
        }
    }
}
