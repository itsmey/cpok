#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "player.h"
#include "game.h"
#include "ui.h"

void player_bet(player_t* player, unsigned short amount) {
  player->cash -= amount;
  player->bet += amount;
  if (game.bet < player->bet)
    game.bet = player->bet;
  LOG("%s bets %u\n", player->name, amount);
}

void player_bank(player_t* player) {
  LOG("%s puts %u to bank\n", player->name, player->bet);
  game.bank += player->bet;
  player->bet = 0;
}

void player_raise(player_t* player) {
  unsigned short raise_sum = game.bet * 2;
  player_bet(player, raise_sum - player->bet);
}

void player_turn(player_t* player) {

  if (player == game_last_player())
    return;

  ui_refresh_msg(M1, "%s turns.", player->name);

  if (player->is_ai) {
     /* TODO: AI */
  } else {
    switch (ui_selector(player)) {
      case CHECK:
        set_msg(M2, "%s: checks.", player->name);
        break;
      case BET:
        set_msg(M2, "%s bets %u", player->name, BIG_BLIND);
        player_bet(player, BIG_BLIND);
        break;
      case RAISE:
        set_msg(M2, "%s raises to %u", player->name, game.bet * 2);
        player_raise(player);
        break;
      case FOLD:
        LOG("%s says fold\n", player->name);
        set_msg(M2, "%s: folds.", player->name);
        player_bank(player);
        player->is_in_game = FALSE;
        break;
      case CALL:
        set_msg(M2, "%s calls %u", player->name, game.bet);
        player_bet(player, game.bet - player->bet);
        break;
    }
  }
  player->is_move_made = TRUE;
}

void player_collect_bank(player_t* player) {
  LOG("%s collects bank of %u\n", player->name, game.bank);
  set_msg(M2, "%s collects bank of %u\n", player->name, game.bank);
  game_collect_bank();
  player->cash += game.bank;
  game.bank = 0;
}

void player_fill_pool(player_t* player, card_t **pool) {
  counter_t i;

  pool[0] = &player->pocket[0];
  pool[1] = &player->pocket[1];

  for(i = 0; i < TABLE_SIZE; i++)
    pool[2+i] = &game.table[i];
}
