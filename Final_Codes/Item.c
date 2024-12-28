#include "Item.h"
#include <stdlib.h>

static ALLEGRO_BITMAP* coin_img;
static ALLEGRO_BITMAP* blue_coin_img;
static ALLEGRO_SAMPLE* coin_sfx;
static ALLEGRO_SAMPLE* blue_coin_sfx;

void init_item()
{
	coin_img = al_load_bitmap("Assets/coins.png");
	blue_coin_img = al_load_bitmap("Assets/BlueCoin.png");
	coin_sfx = al_load_sample("Assets/audio/coin.mp3");
	blue_coin_sfx = al_load_sample("Assets/audio/potion.mp3");
}

void terminate_item()
{
	al_destroy_sample(coin_sfx);
	al_destroy_sample(blue_coin_sfx);
	al_destroy_bitmap(coin_img);
	al_destroy_bitmap(blue_coin_img);
}

Item create_item(ItemType type, Point coord)
{
	Item item;
	memset(&item, 0, sizeof(item));
	item.type = type;
	if (type == Coin) {
		item.image = coin_img;
		item.frameSize = 16;
		item.rect = (RecArea){ coord.x, coord.y, TILE_SIZE, TILE_SIZE };
		item.state = Idle;
		item.pickUpSfx = coin_sfx;
	}
	if (type == Potion) {
		item.image = blue_coin_img;
		item.frameSize = 16;
		item.rect = (RecArea){ coord.x, coord.y, TILE_SIZE, TILE_SIZE };
		item.state = Idle;
		item.pickUpSfx = blue_coin_sfx;
	}
	return item;
}

bool update_item(Item* item, ItemParam* param)
{
	item->animation_tick = ((item->animation_tick) + 1) % 64;
	//Done destroying animation
	if (item->state == Destroying) {
		if (item->animation_tick == 63) 
			return true;
		return false;
	}
	//Not touching player
	if (!(item->rect.x < param->player->coord.x + TILE_SIZE &&
		item->rect.x + item->rect.w > param->player->coord.x &&
		item->rect.y < param->player->coord.y + TILE_SIZE &&
		item->rect.y + item->rect.h > param->player->coord.y)) {
		return false;
	}
	item->state = Destroying;
	item->animation_tick = 0;
	if (item->pickUpSfx) al_play_sample(item->pickUpSfx, SFX_VOLUME, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

	if (item->type == Coin) {
		(*(param->points))++;
	}
	if (item->type == Potion) {
		if (param->player->roombaTimeLeft == 0) param->player->roombaTimeLeft += ROOMBA_TRANSFORM_TIME; //transform time
		param->player->roombaTimeLeft += FPS * 10;
		if(param->player->status != PLAYER_ROOMBA)
			change_player_status(param->player, PLAYER_TRANSFORMING);
	}
	return false;
}

void draw_item(Item* item, const Point* camera)
{
	int dx = item->rect.x - camera->x;
	int dy = item->rect.y - camera->y;
	if (item->type == Coin || item->type == Potion) {
		int xOffset = item->animation_tick / 8;
		int yOffset;
		switch (item->state) {
		case Idle: yOffset = 0; break;
		case Destroying: yOffset = 1; break;
		default: yOffset = 0;
		}
		yOffset *= item->frameSize;
		xOffset *= item->frameSize;
		//game_log("%d %d", xOffset, yOffset);
		al_draw_tinted_scaled_rotated_bitmap_region(item->image,
			xOffset, yOffset, item->frameSize, item->frameSize,
			al_map_rgb_f(1, 1, 1),
			0, 0,
			dx, dy, TILE_SIZE / item->frameSize, TILE_SIZE / item->frameSize,
			0, 0);
	}
#ifdef DRAW_HITBOX
	al_draw_rectangle(dx, dy, dx + item->rect.w, dy + item->rect.h, al_map_rgb(255, 0, 0), 1);
#endif
}

ItemNode* create_item_list()
{
	ItemNode* dummyhead = (ItemNode * )malloc(sizeof(ItemNode));
	dummyhead->next = NULL;
	return dummyhead;
}

void insert_item_list(ItemNode* dummyhead, Item item) {
	ItemNode* newNode = malloc(sizeof(ItemNode));
	newNode->item = item;
	newNode->next = dummyhead->next;
	dummyhead->next = newNode;
}

void update_item_list(ItemNode* dummyhead, ItemParam param)
{
	ItemNode* cur = dummyhead->next;
	ItemNode* prev = dummyhead;
	while (cur != NULL) {
		bool shouldDelete = update_item(&cur->item, &param);
		if (shouldDelete) {
			prev->next = cur->next;
			free(cur);
			cur = prev->next;
		}
		else {
			prev = cur;
			cur = cur->next;
		}
	}
}

void draw_item_list(ItemNode* dummyhead, const Point* camera)
{
	ItemNode* cur = dummyhead->next;
	while (cur != NULL) {
		draw_item(&cur->item, camera);
		cur = cur->next;
	}
}

void destroy_item_list(ItemNode* dummyhead)
{
	while (dummyhead != NULL) {
		ItemNode* del = dummyhead;
		dummyhead = dummyhead->next;
		free(del);
	}
}
