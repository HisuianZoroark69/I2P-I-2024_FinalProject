#ifndef utility_h
#define utility_h
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

# define PI 3.14159265358979323846

/*
     [LOG_ENABLE]

     To print the game log message
     You may comment the define below to boost your game performance
     It's recommended to use this this out before finishing for debugging purpose
*/
#define LOG_ENABLE
/*
     [DRAW_HITBOX]

     DRAW THE HITBOX OF THE CHARACTER AND THE MAP
     BY DRAWING A RED BORDER AROUND THEM
     TO ENABLE, UNCOMMENT THE LINE BELOW
*/
#define DRAW_HITBOX

/*
    [EXTERN VARIABLE]
    To share a same variable to different files
    Remember you can't directly declare variable in header file
    You may change the value variable in utility.c
 */
extern const int FPS; // Frames per second
extern const int SCREEN_W; // Screen Width
extern const int SCREEN_H; // Screen Height
extern const int RESERVE_SAMPLES; // Maximum Audio played at a time
extern const char* GAME_TITLE; // Window title name
extern const int TILE_SIZE; // TILE SIZE Displayed in the game

extern float SFX_VOLUME; // Sound Effects Volume
extern float BGM_VOLUME; // Background Music Volume

extern ALLEGRO_FONT* TITLE_FONT; // 64px
extern ALLEGRO_FONT* P1_FONT; // 48px
extern ALLEGRO_FONT* P2_FONT; // 36px
extern ALLEGRO_FONT* P3_FONT; // 24px

/*
    FUNCTION POINTER
    You can add if you have an unique function pointer
*/
typedef void(*function_void)(void);
typedef void(*function_key)(int keycode);
typedef void(*function_mouse)(int btn, int x, int y, int dz);

/*
    [ENUMERATION & STRUCT DEFINITION]
    You may add new enum or struct definition here
*/

// Direction enumeration
typedef enum _DIRECTION {
    NONE  = 0,
    UP    = 0b0001,
    DOWN  = 0b0010,
    LEFT  = 0b0100,
    RIGHT = 0b1000,
} DIRECTION;

// Integer Point (x, y)
typedef struct _Point{
     int x;
     int y;
 } Point;

// Float Point (x, y)
typedef struct _PointFloat{
    float x;
    float y;
} PointFloat;

// Rectangle (x, y, w=width, h=height)
typedef struct _RecArea {
    float x, y, w, h;
} RecArea;

// log message type
typedef enum logtype_{
    Log,
    Error,
    Warning
} logtype;

typedef struct Scene_{
    char* name;
    function_void init; // initialize
    function_void update; // update scene
    function_void draw; // draw scene
    function_void destroy; // destroy scene
} Scene;

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

// Change Background Music
void change_bgm(char* audio_path, bool continueIfSame);

// Utility Initialization
void init_Util(void);

// Destroy Utility
void destroy_init(void);

// Game Log Message: To print something
void game_log(const char * msg, ...);

// Game Error Message: To Print something and then abort the game
void game_abort(const char * msg, ...);

double number_map(double inStart, double inEnd, double outStart, double outEnd, double value);

int RandNum(int start, int end);

#endif
