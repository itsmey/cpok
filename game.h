#ifndef GAME_H
#define GAME_H

#include "defs.h"
#include "player.h"

typedef enum {INIT, PREFLOP, FLOP, TURN, RIVER, END} round_t;

typedef struct game_t {
  card_t table[TABLE_SIZE];
  unsigned short r_number;
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
player_t* game_next_current();
void game_collect_bank();
void game_choose_winner();
bool_t game_declare_tie();
void game_declare_winner();

/* for test purposes */
void game_deal_specific(const char* player0card1,
                        const char* player0card2,
                        const char* player1card1,
                        const char* player1card2,
                        const char* player2card1,
                        const char* player2card2,
                        const char* player3card1,
                        const char* player3card2,
                        const char* tablecard1,
                        const char* tablecard2,
                        const char* tablecard3,
                        const char* tablecard4,
                        const char* tablecard5);

#endif
