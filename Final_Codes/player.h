#ifndef player_h
#define player_h

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include "utility.h"
#include "map.h"

typedef enum {
    PLAYER_IDLE, PLAYER_WALKING, PLAYER_SHOOTING, PLAYER_DYING, PLAYER_ROOMBA
} PLAYER_STATUS;

typedef struct _PlayerStat {
    int speed; // TODO: CHANGE SPEED SCALED TO THE SIZE OF THE TILES
    int health;
    int atk;
    int atkSpd;
} PlayerStat;

typedef struct _Player{
    Point coord; // coordinate of the player
    double direction; //Radian
    PlayerStat stat;
    
    ALLEGRO_BITMAP* image;
    ALLEGRO_BITMAP* death;
    uint8_t animation_tick; // For animation
    
    float knockback_angle;
    uint8_t knockback_CD;

    PLAYER_STATUS status;
    int flip;
} Player;

Player create_player(char * path, char* death,int frameSize, int row, int col, PlayerStat stat);
void update_player(Player * player, Map * map, int isWeaponShooting);
void draw_player(Player * player, Point cam);
void delete_player(Player * player);
void change_status(Player* player, PLAYER_STATUS status);
void hitPlayer(Player * player, Point enemy_coord, int damage);

#endif /* player_h */
