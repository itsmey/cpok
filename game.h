#ifndef GAME_H
#define GAME_H

#include "defs.h"
#include "card.h"
#include "player.h"

typedef enum {INIT, PREFLOP, FLOP, TURN, RIVER, END} round_t;
typedef enum {CHECK, CALL, BET, RAISE, FOLD} decision_t;

typedef struct game_t {
  card_t table[TABLE_SIZE];
  unsigned short round_number;
  unsigned short bank;
  unsigned short bet;
  short ui_choice;
  player_t players[PLAYERS_COUNT];
  player_t* current;
  round_t round;
  char msg[2][255];
} game_t;

extern game_t game;
extern card_t* global_pool[GLOBAL_POOL_SIZE];
extern card_t* pool[POOL_SIZE];

void game_init();
void game_start();
bool_t game_end_condition();
void game_round();
void game_deal();
void game_blinds();
player_t* game_get_dealer();
void game_change_dealer();
void game_next_part();
bool_t game_equal_bets_condition();
player_t* game_last_player();
void game_next_current();
void game_collect_bank();

#endif
