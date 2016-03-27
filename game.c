#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "defs.h"
#include "game.h"
#include "ui.h"
#include "settings.h"

game_t game;
card_t* global_pool[GLOBAL_POOL_SIZE];
card_t* pool[POOL_SIZE];

/* for testing purposes */
/* dealing special set of cards to test tie conditions */
void tie_test1() {
  game_deal_specific(
    "Ac", "Ah",                    /* player 0 */
    "Kh", "Kc",                    /* player 1 */
    "Kd", "Ks",                    /* player 2 */
    "Ad", "As",                    /* player 3 */
    "2c", "2s", "2h", "3d", "3s"); /* table */
}

void tie_test2() {
  game_deal_specific(
    "2c", "3c",                    /* player 0 */
    "2h", "3h",                    /* player 1 */
    "2d", "3d",                    /* player 2 */
    "2s", "3s",                    /* player 3 */
    "Ac", "As", "Ah", "Ad", "Ks"); /* table */
}

void game_init() {
  counter_t i;
  byte_t next;
  const char* players_names[] = PLAYERS_NAMES;
  const bool_t players_ai[] = PLAYERS_AI;
  byte_t players_ai_pars[][4] = PLAYERS_AI_PARS;

  for (i = 0; i < PLAYERS_COUNT; i++) {
    strcpy(game.players[i].name, players_names[i]);
    next = (i == PLAYERS_COUNT - 1) ? 0 : (i % PLAYERS_COUNT) + 1;
    game.players[i].is_ai = players_ai[i];
    game.players[i].ai.type = AI_DIFF;
    game.players[i].ai.aggr = players_ai_pars[i][1];
    game.players[i].ai.rnds = players_ai_pars[i][2];
    game.players[i].ai.bluf = players_ai_pars[i][3];
    game.players[i].ai.tact = players_ai_pars[i][4];
    game.players[i].next = &(game.players[next]);
    game.players[i].cash = INITIAL_CASH;
    game.players[i].bet = 0;
    game.players[i].is_in_game = FALSE;
    game.players[i].is_dealer = FALSE;
    game.players[i].is_move_made = FALSE;
    game.players[i].is_tie = FALSE;
  }

  ai_init_behaviour(behaviour);

  game.players[rand() % PLAYERS_COUNT].is_dealer = TRUE;
  game.current = NULL;

  game.bank = 0;
  game.bet = 0;
  game.round = INIT;
  game.ui_choice = -1;

  game.r_number = 0;

  set_msg(0, "%s", "");
  set_msg(1, "%s", "");
}

void game_start() {
  settings_init(&settings);
  ui_init();
  settings_show();
  game_init();
  ui_refresh_msg(M1, "%s",
    "You are fucking Shurik. Press any fucking key to start.");

  LOG("%s\n", "initialization done");

  ui_wait_any_key();

  while (!(game_end_condition())) {
    game_round();
  }

  ui_refresh_msg(M2, "%s", "Game over!");
  ui_refresh_msg(M1, "%s", "Press any key to exit.");
  ui_wait_any_key();

  ui_destroy();
}

void game_round() {
  counter_t i;

  LOG("%s\n", "new round");

  game.r_number++;
  ui_refresh_msg(M2, "Round %d: pre-flop", game.r_number);

  ai_clear_behaviour(behaviour);
  game_deal();
  /* tie_test2(); */   /* use this instead of game_deal() to test tie conditions */

  for (i = 0; i < PLAYERS_COUNT; i++) {
    game.players[i].bet = 0;
    game.players[i].is_in_game = (game.players[i].cash > 0);
    game.players[i].is_tie = FALSE;
  }

  LOG("%s\n", "PREFLOP");

  game.round = PREFLOP;
  game_blinds();

  while (game.round != END) {
    while (!game_equal_bets_condition()) {
      player_turn(game.current);
      game_next_current();
    }
    game_next_part();
  }

  LOG("%s\n", "Round ends. Change dealer");
  game_change_dealer();
}

bool_t game_end_condition() {
  counter_t i;
  bool_t result;
  byte_t bankrupts_count = 0;

  for (i = 0; i < PLAYERS_COUNT; i++)
    if (game.players[i].cash <= 0)
      bankrupts_count++;

  result = (bankrupts_count == PLAYERS_COUNT - 1);
  LOG("game end condition: %u\n", result);
  return result;
}

bool_t game_equal_bets_condition() {
  counter_t i;

  for (i = 0; i < PLAYERS_COUNT; i++)
    if ((game.players[i].is_in_game) &&
        (!game_last_player()) &&
        ((game.players[i].bet != game.bet) ||
        (!game.players[i].is_move_made))
       ) {
      LOG("%s\n", "equal bets condition - false. waiting for next player");
      return FALSE;
    }

  LOG("%s\n", "equal bets condition - true");
  return TRUE;
}

void game_deal() {
  counter_t i;
  card_t card;
  card_t** pool[GLOBAL_POOL_SIZE];

  LOG("%s\n", "dealing cards");

  for (i = 0; i < GLOBAL_POOL_SIZE; ++i)
    pool[i] = NULL;

  for (i = 0; i < TABLE_SIZE; i++) {
    card_random_unique(card, (const card_t**)pool, GLOBAL_POOL_SIZE);
    card_copy(card, game.table[i]);
  }
  for (i = 0; i < GLOBAL_POOL_SIZE - TABLE_SIZE; i++) {
    card_random_unique(card, (const card_t**)pool, GLOBAL_POOL_SIZE);
    card_copy(card, game.players[i / 2].pocket[i % 2]);
  }
}

player_t* game_get_dealer() {
  counter_t i;

  FOR_EACH_PLAYER(i)
    if (game.players[i].is_dealer) return &(game.players[i]);

  return NULL;
}

void game_blinds() {
  player_t* dealer = game_get_dealer();

  LOG("paying blinds, dealer is %s\n", dealer->name);

  LOG("%s small blind\n", dealer->next->name);
  player_bet(dealer->next, SMALL_BLIND);
  LOG("%s big blind\n", dealer->next->next->name);
  player_bet(dealer->next->next, BIG_BLIND);

  game.bet = BIG_BLIND;
  game.current = dealer->next->next->next;
  LOG("new current is %s\n", dealer->next->next->next->name);

  ui_refresh_msg(M1, "%s", "Paying blinds...");
  ui_sleep(1);
}

void game_change_dealer() {
  counter_t i;
  player_t* next;

  for (i = 0; i < PLAYERS_COUNT; i++) {
    if (game.players[i].is_dealer) {
      game.players[i].is_dealer = FALSE;
      next = game.players[i].next;
      next->is_dealer = TRUE;
      LOG("dealer is %s\n", next->name);
      break;
    }
  }
}

void game_next_part() {
  counter_t i;
  player_t* dealer = game_get_dealer();

  if (game_last_player()) {
    game_declare_winner();
    game.round = END;
    return;
  }

  switch (game.round) {
    case INIT:
      break;
    case PREFLOP:
      game_collect_bank();
      LOG("%s\n", "changing round to FLOP");
      set_msg(M2, "Round %d: flop", game.r_number);
      game.round = FLOP;
      break;
    case FLOP:
      game_collect_bank();
      LOG("%s\n", "changing round to TURN");
      set_msg(M2, "Round %d: turn", game.r_number);
      game.round = TURN;
      break;
    case TURN:
      game_collect_bank();
      LOG("%s\n", "changing round to RIVER");
      set_msg(M2, "Round %d: river", game.r_number);
      game.round = RIVER;
      break;
    case RIVER:
      game_collect_bank();
      LOG("%s\n", "round ends");
      game_choose_winner();
      game.round = END;
      break;
    case END:
      break;
  }

  for (i = 0; i < PLAYERS_COUNT; i++) {
    game.players[i].bet = 0;
    game.players[i].is_move_made = FALSE;
  }
  game.bet = 0;

  game.current = dealer->next;
}

void game_collect_bank() {
  counter_t i;
  LOG("bank is %u\n", game.bank);
  FOR_EACH_PLAYER(i) {
    if (game.players[i].bet > 0) {
      LOG("collecting %u from player %s\n",
        game.players[i].bet, game.players[i].name);
      player_bank(&game.players[i]);
    }
  }
  LOG("bank is %u\n", game.bank);
}

player_t* game_last_player() {
  counter_t i;
  byte_t count = 0;
  player_t* player = NULL;

  FOR_EACH_PLAYER(i)
    if (game.players[i].is_in_game) {
      count++;
      player = &game.players[i];
    }

  if (count == 1)
    return player;
  else
    return NULL;
}

player_t* game_next_current() {
  player_t* last_player = game_last_player();

  if (last_player) {
    LOG("last player is %s\n", last_player->name);
    game.current = last_player;
  } else {
    do {
      game.current = game.current->next;
    } while (!game.current->is_in_game);
    LOG("next player is %s\n", game.current->name);
  }
  return game.current;
}

void game_choose_winner() {
  card_t *pool[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  player_t *yet_winner = NULL;

  set_msg(M2, "%s", "Revealing cards...");

  do {
    yet_winner = player_reveal(game.current, yet_winner, pool);
    game_next_current();
  } while ( !game_last_player() && !game_tie_condition() );

  if (!game_declare_tie()) {
    game_declare_winner();
  }
}

bool_t game_declare_tie() {
  counter_t i, msgs;
  counter_t ties;
  unsigned short part_size;
  char header[255];
  char msg[4][W_INFO_WIDTH-2];

  ties = 0;

  FOR_EACH_PLAYER(i)
    if (game.players[i].is_tie) ties++;

  LOG("ties %d\n", ties);

  if (!ties) return FALSE;

  part_size = game.bank / ties;

  game_collect_bank();
  sprintf(header, "[ Round %u summary ]", game.r_number);

  msgs = 0;

  FOR_EACH_PLAYER(i) {
    if (game.players[i].is_tie) {
      sprintf(msg[msgs++], "%s collects part of %u.", game.players[i].name, part_size);
      player_collect_part(&game.players[i], part_size);
    }
  }

  ui_info_window(header, msg[0], ties);

  return TRUE;
}

void game_declare_winner() {
  char header[255];
  char msg[1][W_INFO_WIDTH-2];
  player_t *last = game_last_player();

  game_collect_bank();
  sprintf(header, "[ Round %u summary ]", game.r_number);
  sprintf(msg[0], "%s collects bank of %u.", last->name, game.bank);
  ui_info_window(header, msg[0], 1);
  player_collect_bank(last);
}

bool_t game_tie_condition() {
  counter_t i;

  FOR_EACH_PLAYER(i) {
    if (game.players[i].is_in_game) {
      if (!game.players[i].is_tie)
        return FALSE;
    }
  }

  return TRUE;
}

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
                        const char* tablecard5) {
  card_t card;

  LOG("%s\n", "dealing cards");

  #define COPY(str, to)   \
    card_fill(card, str); \
    card_copy(card, to);

  COPY(player0card1, game.players[0].pocket[0]);
  COPY(player0card2, game.players[0].pocket[1]);
  COPY(player1card1, game.players[1].pocket[0]);
  COPY(player1card2, game.players[1].pocket[1]);
  COPY(player2card1, game.players[2].pocket[0]);
  COPY(player2card2, game.players[2].pocket[1]);
  COPY(player3card1, game.players[3].pocket[0]);
  COPY(player3card2, game.players[3].pocket[1]);
  COPY(tablecard1, game.table[0]);
  COPY(tablecard2, game.table[1]);
  COPY(tablecard3, game.table[2]);
  COPY(tablecard4, game.table[3]);
  COPY(tablecard5, game.table[4]);

  #undef COPY
}
