#ifndef PLAYER_H
#define PLAYER_H

#include "defs.h"
#include "card.h"

typedef struct player_t {
  char name[16];
  bool_t is_ai;
  bool_t is_dealer;
  bool_t is_in_game;
  card_t pocket[HAND_SIZE];
  unsigned short cash;
  unsigned short bet;
  struct player_t* next;

} player_t;

void player_bet(player_t* player, unsigned short amount);
void player_turn(player_t* player);
void player_collect_bank(player_t* player);

#endif
