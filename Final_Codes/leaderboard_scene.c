#include "leaderboard_scene.h"
#include <allegro5\allegro.h>
#include <stdlib.h>
#include "UI.h"
#include "menu_scene.h"
#define ENTRIES_COUNT 6

ALLEGRO_CONFIG* cfg;
ALLEGRO_EVENT_QUEUE* keyboard;
ALLEGRO_FONT* font;
Button MainMenu;
Button AddEntry;
const char* filename = "totally_not_a_leaderboard_file.cfg";
const char* section = "leaderboard";
char names[ENTRIES_COUNT][10];
int levels[ENTRIES_COUNT];
int levelsMin, curLevel;
int addingEntry, addingCharPos;
int blinkTick;

void read_leaderboard() {
	memset(names, 0, sizeof(names));
	memset(levels, 0, sizeof(levels));

	ALLEGRO_CONFIG_ENTRY* iterator;
	const char* buffer;
	buffer = al_get_first_config_entry(cfg, section, &iterator);
	if(buffer)
		memcpy(names[0], buffer, sizeof(char) * strlen(buffer));
	if (strlen(names[0]) )
		levels[0] = strtol(al_get_config_value(cfg, section, names[0]), NULL, 10);
	levelsMin = levels[0];
	for (int i = 1; i < ENTRIES_COUNT && names[i - 1] != NULL; i++) {
		buffer = al_get_next_config_entry(&iterator);
		if(buffer != NULL)
			memcpy(names[i], buffer, sizeof(char) * strlen(buffer));
		if (strlen(names[i]))
			levels[i] = strtol(al_get_config_value(cfg, section, names[i]), NULL, 10);
		if (levels[i] < levelsMin) levelsMin = levels[i];
	}
}

void save_leaderboard() {
	al_remove_config_section(cfg, section);
	al_add_config_section(cfg, section);
	char buffer[10];
	for (int i = 0; i < ENTRIES_COUNT; i++) {
		if (!strlen(names[i])) continue;
		//If value is duplicated, dont add it.
		//The list is ordered descendingly, so the duplicated one is guaranteed to be smaller.
		if (al_get_config_value(cfg, section, names[i])) continue;
		itoa(levels[i], buffer, 10);
		al_set_config_value(cfg, section, names[i], buffer);
	}
	al_save_config_file(filename, cfg);
}

//A function to get the min and max score in the leaderboard, for use outside of the scene
//Pointers could be null
void getMinMaxScore(int* minScore, int* maxScore) {
	cfg = al_load_config_file(filename);
	if (!cfg) {
		//Initialize config file if not exist
		cfg = al_create_config();
		al_add_config_section(cfg, section);
		al_save_config_file(filename, cfg);
	}
	read_leaderboard();
	if (minScore != NULL) *minScore = 100;
	if (maxScore != NULL) *maxScore = -1;
	for (int i = 0; i < ENTRIES_COUNT; i++) {
		if (maxScore != NULL && *maxScore < levels[i]) *maxScore = levels[i];
		if (minScore != NULL && *minScore > levels[i]) *minScore = levels[i];
	}
	al_destroy_config(cfg);
}

void init() {
	MainMenu = button_create(SCREEN_W / 2 - 150, 675, 300, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
	AddEntry = button_create(SCREEN_W / 2 - 150, 550, 300, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

	font = al_load_font("Assets/YouDiedFont.otf", 35, 0);
	keyboard = al_create_event_queue();
	al_register_event_source(keyboard, al_get_keyboard_event_source());
	cfg = al_load_config_file(filename);
	if (!cfg) {
		cfg = al_create_config();
		al_add_config_section(cfg, section);
		al_save_config_file(filename, cfg);
	}
	read_leaderboard();

	addingCharPos = 0;
	addingEntry = -1;
}
void destroy() {
	save_leaderboard();
	destroy_button(&MainMenu);
	destroy_button(&AddEntry);
	al_destroy_event_queue(keyboard);
	al_destroy_config(cfg);
}
void updateKeyboard() {
	ALLEGRO_EVENT e;
	if (!al_get_next_event(keyboard, &e)) return;
	if (e.type != ALLEGRO_EVENT_KEY_CHAR) return;
	if (addingEntry == -1 || addingEntry == ENTRIES_COUNT) return;
	if (e.keyboard.keycode == ALLEGRO_KEY_ENTER || addingCharPos > 8) {
		addingEntry = ENTRIES_COUNT; // entry added
		addingCharPos = 0;
		save_leaderboard();
		read_leaderboard();
		return;
	}
	if (e.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
		if (addingCharPos > 0) addingCharPos--;
		names[addingEntry][addingCharPos] = 0;
		return;
	}
	names[addingEntry][addingCharPos++] = e.keyboard.unichar;
}
void update() {
	blinkTick = (blinkTick + 1) % 60;
	update_button(&MainMenu);
	if (addingEntry == -1 && curLevel > levelsMin)
		update_button(&AddEntry);
	if (mouseButtonUp & 1 && MainMenu.hovered) {
		change_scene(create_menu_scene());
		return;
	}
	if (mouseButtonUp & 1 && AddEntry.hovered && addingEntry < ENTRIES_COUNT) {
		//Find entry place
		for (int i = 0; i < ENTRIES_COUNT; i++) {
			if (levels[i] < curLevel) {
				addingEntry = i;
				break;
			}
		}
		//Making space for the new entry
		for (int i = ENTRIES_COUNT - 1; i > addingEntry; i--) {
			memcpy(names[i], names[i - 1], sizeof(names[i]));
			levels[i] = levels[i - 1];
		}
		memset(names[addingEntry], 0, sizeof(names[addingEntry]));
		levels[addingEntry] = curLevel;
		addingCharPos = 0;
	}
	updateKeyboard();
}
void draw() {
	draw_button(MainMenu, "Main menu");
	if (addingEntry == -1 && curLevel > levelsMin)
		draw_button(AddEntry, "Add entry");
	if(curLevel > 0) //will not print if accessed from main menu
		al_draw_textf(font, al_map_rgb_f(1, 1, 1), 50, 10, 0, "Your level: %d", curLevel);
	for (int i = 0; i < ENTRIES_COUNT; i++) {
		if (names[i] == NULL) continue;
		al_draw_textf(font, al_map_rgb_f(1, 1, 1), 150, 50 + i * 50, 0,
				      "%d.", i + 1);
		al_draw_textf(font, al_map_rgb_f(1, 1, 1), 190, 50 + i * 50, 0,
			"%s", names[i]);
		al_draw_textf(font, al_map_rgb_f(1, 1, 1), 600, 50 + i * 50, 0,
			"%d", levels[i]);
	}
	//Draw the blinky thingy when editing text
	if (blinkTick / 30 && addingEntry != -1 && addingEntry != ENTRIES_COUNT) {
		int charWidth, charHeight, charX, charY;
		al_get_text_dimensions(font, names[addingEntry], &charX, &charY, &charWidth, &charHeight);
		//Accomodate the space character
		if (addingCharPos > 0 && names[addingEntry][addingCharPos - 1] == ' ') charWidth += 10;
		charHeight = 42;
		int dx = 190 + charWidth;
		int dy = 50 + addingEntry * 50;
		al_draw_filled_rectangle(dx, dy, dx + 2, dy + abs(charHeight) - 7, al_map_rgb_f(1, 1, 1));
	}
}

Scene create_leaderboard_scene(int currentLevel)
{
	Scene ret;
	ret.name = "leaderboard_scene";
	ret.init = init;
	ret.destroy = destroy;
	ret.draw = draw;
	ret.update = update;
	
	curLevel = currentLevel;
	return ret;
}
