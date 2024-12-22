#ifndef UI_H
#define UI_H
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <string.h>
#include "utility.h"

typedef struct _Button {
	int x, y, w, h;

	ALLEGRO_BITMAP* default_img;
	ALLEGRO_BITMAP* hovered_img;

	bool hovered;
}Button;

typedef struct _Slider {
	int x, y, w, h;
	float valMin, valMax;
	float* bindedValue;
	ALLEGRO_BITMAP* default_img;
}Slider;

Button button_create(int x, int y, int w, int h, const char* default_image_path, const char* hovered_image_path);
Slider slider_create(int x, int y, int w, int h, float min, float max, float* bindedValue, const char* default_img_path);
void draw_button(Button button, const char* text);
void draw_slider(Slider* slider, ALLEGRO_COLOR ball_color);
void update_button(Button* button);
void update_slider(Slider* slider);
void destroy_button(Button* button);
void destroy_slider(Slider* slider);

#endif