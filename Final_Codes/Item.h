#pragma once
#include<allegro5\allegro.h>
#include "utility.h"
#include "UI.h"
#include "game.h"
#include "player.h"

typedef enum _ItemType {
	Coin, ItemTypeCount
}ItemType;

typedef enum _ItemState {
	Idle, Destroying
} ItemState;

typedef struct _ItemParam {
	Player* player;
	int* points;
} ItemParam;

typedef struct _Item {
	ALLEGRO_BITMAP* image;
	ItemType type;
	ItemState state;
	int animation_tick;
	int frameSize;
	RecArea rect;
}Item;

typedef struct _ItemNode {
	Item item;
	struct _ItemNode* next;
}ItemNode;

void init_item();
void terminate_item();

Item create_item(ItemType type, Point coord);
//bool update_item(Item* item, ItemParam* param);
//void draw_item(Item* item);
ItemNode* create_item_list();
void insert_item_list(ItemNode* dummyhead, Item item);
void update_item_list(ItemNode* dummyhead, ItemParam param);
void draw_item_list(ItemNode* dummyhead, const Point* camera);
void destroy_item_list(ItemNode* dummyhead);

