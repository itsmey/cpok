#include <stdlib.h>

#include "ai.h"
#include "ai_data.h"
#include "ui.h"
#include "pok.h"

behaviour_t behaviour[PLAYERS_COUNT];

decision_t ai_decision(player_t *player) {
  decision_t d;

  player->ai.strength = ai_strength(player);

  switch (AI_DIFF) {
    case DUMB: d = ai_decision_dumb(player); break;
    case SMARTER: d = ai_decision_dumb(player); break;
  }

  return d;
}

decision_t ai_decision_dumb(player_t *player) {
  byte_t mark1, mark2, indicator;
  decision_t decision;

  LOG("Player %s strength = %u\n", player->name, player->ai.strength);
  LOG("Player %s aggr = %u\n", player->name, player->ai.aggr);

  mark1 = 100 - player->ai.strength;
  mark2 = mark1 + player->ai.strength * (100 - player->ai.aggr) / 100;

  indicator = rand() % 100 + 1;  /* 1..100 */

  LOG("mark1 %u mark2 %u indicator %u\n", mark1, mark2, indicator);

  if (indicator < mark1) {
    decision = can_check(player) ? CHECK : FOLD;
  } else
  if (indicator < mark2) {
    decision = can_bet(player) ? BET : CALL;
  } else {
    decision = can_raise(player) ? RAISE : (can_bet(player) ? BET : CALL);
  }

  ui_sleep(2);
  /*ui_wait_any_key();*/
  return decision;
}

unsigned short ai_strength(player_t *player) {
  byte_t strength, value1, value2;
  short diff;
  card_t *p[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  byte_t size;
  combo_t combo;

  LOG("Calculating strength for player %s\n", player->name);

  if (game.round == PREFLOP) {
    LOG("Preflop%s\n", "");
    value1 = card_value(&player->pocket[0]);
    value2 = card_value(&player->pocket[1]);
    strength = preflop_strength[value1][value2];
    LOG("base strength is %u\n", strength);
    if (player->pocket[0][SUIT] == player->pocket[1][SUIT]) {
      LOG("bonus for equal suits%s\n", "");
      strength += 10;
    }
    diff = value1 - value2;
    if ((diff == 1) || (diff == -1)) {
      LOG("bonus for adjacency%s\n", "");
      strength += 5;
    }
  } else {
    size = player_fill_pool(player, p);
    combo = pok_resolve(p, size);

    strength = combo_strength[(byte_t)combo.kind][(byte_t)game.round - 2];
    LOG("base strength is %u\n", strength);
  }

  return (strength > 100) ? 100 : strength;
}

void ai_init_behaviour(behaviour_t *b) {
  counter_t i;

  FOR_EACH_PLAYER(i) {
    behaviour[i].player = &game.players[i];
    behaviour[i].raises = 0;
    behaviour[i].bets = 0;
    behaviour[i].calls = 0;
    behaviour[i].checks = 0;
    behaviour[i].folds = 0;
    behaviour[i].raises_total = 0;
    behaviour[i].bets_total = 0;
    behaviour[i].calls_total = 0;
    behaviour[i].checks_total = 0;
    behaviour[i].folds_total = 0;
    behaviour[i].last = FOLD;
  }
}

void ai_clear_behaviour(behaviour_t *b) {
  counter_t i;

  FOR_EACH_PLAYER(i) {
    behaviour[i].raises = 0;
    behaviour[i].bets = 0;
    behaviour[i].calls = 0;
    behaviour[i].checks = 0;
    behaviour[i].folds = 0;
  }
}

void ai_update_behaviour(player_t *p, decision_t d, behaviour_t *b) {
  counter_t i;

  FOR_EACH_PLAYER(i) {
    if (p == &game.players[i]) {
      switch (d) {
        case FOLD:
          behaviour[i].folds++;
          behaviour[i].folds_total++;
        break;
        case BET:
          behaviour[i].bets++;
          behaviour[i].bets_total++;
        break;
        case CALL:
          behaviour[i].calls++;
          behaviour[i].calls_total++;
        break;
        case CHECK:
          behaviour[i].checks++;
          behaviour[i].checks_total++;
        break;
        case RAISE:
          behaviour[i].raises++;
          behaviour[i].raises_total++;
        break;
      }
      behaviour[i].last = d;
    }
  }
}
