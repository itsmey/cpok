#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "player.h"
#include "game.h"
#include "ui.h"

void player_bet(player_t* player, unsigned short amount) {
  player->cash -= amount;
  game.bank += amount;
  player->bet += amount;
  LOG("%s bets %u\n", player->name, amount);
}

void player_turn(player_t* player) {
  unsigned short raise_sum;

  ui_refresh_msg(player->name);

  if (player->is_ai) {
     /* TODO: AI */
  } else {
    switch (ui_selector(player)) {
      case CHECK:
        LOG("%s says check\n", player->name);
        break;
      case BET:
        LOG("%s says bet\n", player->name);
        player_bet(player, BIG_BLIND);
        break;
      case RAISE:
        LOG("%s says raise\n", player->name);
        raise_sum = game.bet * 2;
        player_bet(player, raise_sum - player->bet);
        game.bet = raise_sum;
        break;
      case FOLD:
        LOG("%s says fold\n", player->name);
        player->cash += player->bet;
        player->is_in_game = FALSE;
        break;
      case CALL:
        LOG("%s says call\n", player->name);
        player_bet(player, game.bet - player->bet);
        break;
    }
  }
}

void player_collect_bank(player_t* player) {
  player->cash += game.bank;
  player->cash += player->bet;

  game.bank = 0;
  player->bet = 0;
}
