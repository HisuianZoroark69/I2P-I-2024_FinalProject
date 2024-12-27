#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "utility.h"

const int FPS = 60; 
const int SCREEN_W = 800;
const int SCREEN_H = 800;
const int RESERVE_SAMPLES = 10;
const char* GAME_TITLE = "NTHU-RPG Adventure";
const char* log_file = "log.txt";
const char* font_file = "Assets/Minecraft.ttf";
char currentAudio[1000];
const int TILE_SIZE = 64;

ALLEGRO_SAMPLE* BGM = NULL;
float SFX_VOLUME = 0.5f;
float BGM_VOLUME = 0.5f;

ALLEGRO_FONT* TITLE_FONT;
ALLEGRO_FONT* P1_FONT;
ALLEGRO_FONT* P2_FONT;
ALLEGRO_FONT* P3_FONT;

pcg32_random_t rng;

void change_bgm(char* audio_path, bool continueIfSame) {
    if (continueIfSame && strcmp(currentAudio, audio_path) == 0) {
        game_log("Continue playing the bgm");
        return;
    }
    strcpy(currentAudio, audio_path);
    if (BGM) {
        al_destroy_sample(BGM);
        BGM = NULL;
    }

    BGM = al_load_sample(audio_path);
    if (!BGM) {
        game_log("No BGM File found [%s], no sound will be played", audio_path);
    }
    else {  
        al_play_sample(BGM, BGM_VOLUME, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
    }
}

void init_Util(void){
    TITLE_FONT = al_load_font(font_file, 64, 0);
    if(!TITLE_FONT){
        game_abort("Failed to load Title Font");
    }
    
    P1_FONT = al_load_font(font_file, 48, 0);
    if(!P1_FONT){
        game_abort("Failed to load P1 Font");
    }
    
    P2_FONT = al_load_font(font_file, 36, 0);
    if(!P2_FONT){
        game_abort("Failed to load P2 Font");
    }
    
    P3_FONT = al_load_font(font_file, 24, 0);
    if(!P3_FONT){
        game_abort("Failed to load P3 Font");
    }
}

void destroy_init(void) {
    al_destroy_font(TITLE_FONT);
    al_destroy_font(P1_FONT);
    al_destroy_font(P2_FONT);
    al_destroy_font(P3_FONT);
}

/*
    [GAME LOG]

    You can disable the log by remove the "#define LOG_ENABLE" in utility.h
    It's not recommended if you are still in progress on making this game
    You may do it after finishing it for performance boost purpose
*/
void print_log(const char * msg, va_list arg, logtype type){
#ifdef LOG_ENABLE
    // Open File
    static bool firstOpen = true;
    FILE* f;

    if (firstOpen) {
        f = fopen(log_file, "w");
        firstOpen = false;
    }
    else {
        f = fopen(log_file, "a");
    }
    
    switch(type){
        case Log:
            printf("[LOG] ");
            if(f != NULL)
                fprintf(f, "[LOG] ");
            break;
        case Error:
            printf("[Error] ");
            if(f != NULL)
                fprintf(f, "[Error] ");
            break;
        case Warning:
            printf("[Warning] ");
            if(f != NULL)
                fprintf(f, "[Warning] ");
        default:
            printf("[???] ");
            if(f != NULL)
                fprintf(f, "[???] ");
            break;
    }
    //Cloning arg before using
    va_list fArg;
    va_copy(fArg, arg);

    vprintf(msg, arg);
    printf("\n");

    if(f != NULL){
        vfprintf(f, msg, fArg);
        fprintf(f, "\n");
        fclose(f);
    }
    va_end(fArg);

#endif
}

void game_log(const char * msg, ...){
#ifdef LOG_ENABLE
    va_list args;
    va_start(args, msg);
    print_log(msg, args, Log);
    va_end(args);
#endif
}

void game_abort(const char * msg, ...){
#ifdef LOG_ENABLE
    va_list args;
    va_start(args, msg);
    print_log(msg, args, Error);
    va_end(args);
#endif
    // Abort the game if there is an error
    exit(1);
}

double number_map(double inStart, double inEnd, double outStart, double outEnd, double value) {
    if (value < inStart) return outStart;
    if (value > inEnd) return outEnd;
    double slope = 1.0 * (outEnd - outStart) / (inEnd - inStart);
    return outStart + slope * (value - inStart);
}

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

uint32_t pcg32_random_r(pcg32_random_t* rng)
{
    uint64_t oldstate = rng->state;
    // Advance internal state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc | 1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

void RandomInit() {
    srand(time(NULL));
    rng.state = rand();
    rng.inc = rand();
}

int RandNum(int start, int end) {
    return pcg32_random_r(&rng) % (end - start + 1) + start;
}

// To print with a Warning Sign
void game_warning(const char * msg, ...){
#ifdef LOG_ENABLE
    va_list args;
    va_start(args, msg);
    print_log(msg, args, Warning);
    va_end(args);
#endif
}
