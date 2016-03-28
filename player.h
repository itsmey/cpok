#ifndef PLAYER_H
#define PLAYER_H

#include "defs.h"
#include "card.h"
#include "ai.h"

typedef struct player_t {
  char name[16];
  bool_t is_ai;
  bool_t is_dealer;
  bool_t is_in_game;
  bool_t is_move_made;
  bool_t is_tie;
  bool_t is_cards_opened;
  card_t pocket[HAND_SIZE];
  ai_t ai;
  unsigned short cash;
  unsigned short bet;
  struct player_t* next;

} player_t;

void player_bet(player_t* player, unsigned short amount);
void player_turn(player_t* player);
void player_collect_bank(player_t* player);
void player_collect_part(player_t* player, unsigned short part);
void player_bank(player_t* player);
void player_raise(player_t* player);
byte_t player_fill_pool(player_t* player, card_t **pool);
player_t* player_reveal(player_t *player, player_t *yet_winner, card_t **pool);

bool_t can_bet(player_t* player);
bool_t can_call(player_t* player);
bool_t can_check(player_t* player);
bool_t can_raise(player_t* player);

#endif
