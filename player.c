#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "player.h"
#include "game.h"
#include "ui.h"
#include "pok.h"
#include "settings.h"

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

player_t* player_reveal(player_t *player, player_t *yet_winner, card_t **pool) {
  counter_t i;
  card_t *p[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  char str_combo[255];

  player_fill_pool(player, p);
  card_combo_to_text(str_combo, pok_resolve(p));

  if (!yet_winner) {
    LOG("yet_winner now is %s\n", yet_winner->name);
    for(i = 0; i < POOL_SIZE; i++)
      pool[i] = p[i];
    set_msg(M1, "%s reveals %s. Press any key.", player->name, str_combo);
    ui_refresh();
    ui_wait_any_key();
    return player;
  }

  if (pok_compare(pool, p) == pool) {
    /* we lose! fold */
    set_msg(M1, "%s folds. Press any key.", player->name);
    player_bank(player);
    player->is_in_game = FALSE;
    ui_refresh();
    ui_wait_any_key();
    return yet_winner;
  }

  if (pok_compare(pool, p) == p) {
    /* we win (for now) */
    yet_winner->is_in_game = FALSE;
    LOG("%s lefts the game\n", yet_winner->name);
    yet_winner = player;
    for(i = 0; i < POOL_SIZE; i++)
      pool[i] = p[i];
    set_msg(M1, "%s reveals %s. Press any key.", player->name, str_combo);
    ui_refresh();
    ui_wait_any_key();
    return player;
  }

  if (pok_compare(pool, p) == NULL) {
    /* we tie */
    player->is_tie = TRUE;
    set_msg(M1, "%s reveals %s. Press any key.", player->name, str_combo);
    ui_refresh();
    ui_wait_any_key();
    return yet_winner;
  }

  return NULL;  /* unreachable */
}
