#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "player.h"
#include "game.h"
#include "ui.h"

void player_bet(player_t* player, unsigned short amount) {
  player->cash -= amount;
  /*game.bank += amount;*/
  player->bet += amount;
  LOG("%s bets %u\n", player->name, amount);
}

void player_turn(player_t* player) {
  unsigned short raise_sum;

  if (player == game_last_player())
    return;

  ui_refresh_msg(M1, "%s turns.", player->name);

  if (player->is_ai) {
     /* TODO: AI */
  } else {
    switch (ui_selector(player)) {
      case CHECK:
        LOG("%s says check\n", player->name);
        set_msg(M2, "%s: checks.", player->name);
        break;
      case BET:
        LOG("%s says bet\n", player->name);
        set_msg(M2, "%s bets %u", player->name, BIG_BLIND);
        player_bet(player, BIG_BLIND);
        if (game.bet < BIG_BLIND)
          game.bet = BIG_BLIND;
        break;
      case RAISE:
        LOG("%s says raise\n", player->name);
        set_msg(M2, "%s raises to %u", player->name, game.bet * 2);
        raise_sum = game.bet * 2;
        player_bet(player, raise_sum - player->bet);
        game.bet = raise_sum;
        break;
      case FOLD:
        LOG("%s says fold\n", player->name);
        set_msg(M2, "%s: folds.", player->name);
        game.bank += player->bet;
        player->is_in_game = FALSE;
        break;
      case CALL:
        LOG("%s says call\n", player->name);
        set_msg(M2, "%s calls %u", player->name, game.bet);
        player_bet(player, game.bet - player->bet);
        break;
    }
  }
  player->is_move_made = TRUE;
}

void player_collect_bank(player_t* player) {
  player->cash += game.bank;
  /*player->cash += player->bet;*/

  LOG("%s collects bank of %u\n", player->name, game.bank);

  game.bank = 0;
  player->bet = 0;
}
