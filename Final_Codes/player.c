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
        int next_x = player->coord.x + player->speed * cos(player->knockback_angle);
        int next_y = player->coord.y + player->speed * sin(player->knockback_angle);
        Point next;
        next = (Point){next_x, player->coord.y};

        if(!isCollision(player, map)){
            player->coord = next;
        }

        next = (Point){player->coord.x, next_y};
        if(!isCollision(player, map)){
            player->coord = next;
        }
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
    DIRECTION collisionDir = isCollision(player, map);

    if(keyState[ALLEGRO_KEY_W]) dirY -= 1;
    if(keyState[ALLEGRO_KEY_S]) dirY += 1;
    if(keyState[ALLEGRO_KEY_A]) dirX -= 1;
    if(keyState[ALLEGRO_KEY_D]) dirX += 1;

    if(dirX || dirY){
        player->direction = atan2(dirY, dirX);
        player->coord.x += player->speed * cos(player->direction);
        player->coord.y += player->speed * sin(player->direction);
        //game_log("Player move with direction %d", (int)(player->direction * 180 / PI));
    }
    if(!(collisionDir & UP)) dirY -= 1;
    if(!(collisionDir & DOWN)) dirY += 1;
    if(!(collisionDir & LEFT)) player->coord.x = round((float)original.x / (float)TILE_SIZE) * TILE_SIZE;;
    if(!(collisionDir & RIGHT)) dirX ;

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
    //check out of bounds (Checked in isWalkable)
    // if (corners[0].x < 0         ||
    //     corners[0].y < 0         ||
    //     corners[3].x >= map->col ||
    //     corners[3].x >= map->row)
    //     return NONE;

    /*

        [TODO HACKATHON 2-1]

        Check every corner of enemy if it's collide or not

        We check every corner if it's collide with the wall/hole or not
        You can activate DRAW_HITBOX in utility.h to see how the collision work
    */
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
        player->knockback_CD = 32;

        player->health -= 10;
        if(player->health <= 0){

            player->health = 0;
            player->status = PLAYER_DYING;
        }
    }
}
