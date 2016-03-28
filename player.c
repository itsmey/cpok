#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "player.h"
#include "game.h"
#include "ui.h"
#include "pok.h"
#include "settings.h"
#include "ai.h"


void player_bet(player_t* player, unsigned short amount) {
  amount = (player->cash >= amount) ? amount : player->cash;
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
  decision_t decision;

  if (player == game_last_player())
    return;

  if (player->cash <= 0) {
    /* player have no cash and only watching */
    ui_refresh_msg(M1, "%s has no cash to turn.", player->name);
    ui_sleep(game.sleep_time);
  } else {
    ui_refresh_msg(M1, "%s is thinking...", player->name);

    if (player->is_ai)
      ui_sleep(game.sleep_time);

    decision = (player->is_ai) ? ai_decision(player) : ui_selector(player);

    switch (decision) {
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

    ai_update_behaviour(player, decision, behaviour);
  }

  player->is_move_made = TRUE;
  /*FOR_EACH_PLAYER(i) {
    LOG("%s's statistics: %u %u %u %u %u %u %u %u %u %u %u\n",
      game.players[i].name,
      behaviour[i].raises,
      behaviour[i].bets,
      behaviour[i].calls,
      behaviour[i].checks,
      behaviour[i].folds,
      behaviour[i].raises_total,
      behaviour[i].bets_total,
      behaviour[i].calls_total,
      behaviour[i].checks_total,
      behaviour[i].folds_total,
      behaviour[i].last);
  }*/
}

void player_collect_bank(player_t* player) {
  LOG("%s collects bank of %u\n", player->name, game.bank);
  set_msg(M2, "%s collects bank of %u\n", player->name, game.bank);
  game_collect_bank();
  player->cash += game.bank;
  game.bank = 0;
}

void player_collect_part(player_t* player, unsigned short part) {
  LOG("%s collects part of %u\n", player->name, part);
  set_msg(M2, "%s collects part of %u\n", player->name, game.bank);
  game_collect_bank();
  player->cash += part;
  game.bank -= part;
}

byte_t player_fill_pool(player_t* player, card_t **pool) {
  counter_t i;
  byte_t revealed_cards;

  switch (game.round) {
    case INIT: revealed_cards = 0; break;
    case PREFLOP: revealed_cards = 0; break;
    case FLOP: revealed_cards = 3; break;
    case TURN: revealed_cards = 4; break;
    case RIVER: revealed_cards = 5; break;
    case END: revealed_cards = 0; break;
  }

  pool[0] = &player->pocket[0];
  pool[1] = &player->pocket[1];

  for(i = 0; i < revealed_cards; i++)
    pool[HAND_SIZE+i] = &game.table[i];

  return HAND_SIZE + revealed_cards;
}

player_t* player_reveal(player_t *player, player_t *yet_winner, card_t **pool) {
  counter_t i;
  card_t *p[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  char str_combo[255];

  player_fill_pool(player, p);
  card_combo_to_text(str_combo, pok_resolve(p, POOL_SIZE));

  if (!yet_winner) {
    LOG("yet_winner now is %s\n", player->name);
    for(i = 0; i < POOL_SIZE; i++)
      pool[i] = p[i];
    set_msg(M1, "%s reveals %s. Press any key.", player->name, str_combo);
    player->is_cards_opened = TRUE;
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

    /* clear previous ties */
    FOR_EACH_PLAYER(i) {
      if (game.players[i].is_tie) {
        game.players[i].is_in_game = FALSE;
        game.players[i].is_tie = FALSE;
      }
    }

    LOG("%s lefts the game\n", yet_winner->name);
    yet_winner = player;
    for(i = 0; i < POOL_SIZE; i++)
      pool[i] = p[i];
    set_msg(M1, "%s reveals %s. Press any key.", player->name, str_combo);
    player->is_cards_opened = TRUE;
    ui_refresh();
    ui_wait_any_key();
    return player;
  }

  if (pok_compare(pool, p) == NULL) {
    /* we tie */
    player->is_tie = TRUE;
    yet_winner->is_tie = TRUE;
    set_msg(M1, "%s reveals %s. Press any key.", player->name, str_combo);
    player->is_cards_opened = TRUE;
    ui_refresh();
    ui_wait_any_key();
    return yet_winner;
  }

  return NULL;  /* unreachable */
}

bool_t can_bet(player_t* player) {
  return ((player->bet == 0) && (game.bet == 0));
}

bool_t can_call(player_t* player) {
  return (player->bet < game.bet);
}

bool_t can_check(player_t* player) {
  return (player->bet == game.bet);
}

bool_t can_raise(player_t* player) {
  return (game.bet != 0);
}
