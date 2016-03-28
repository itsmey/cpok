#ifndef AI_H
#define AI_H

#include "defs.h"
#include "settings.h"
#include "card.h"

typedef struct player_t player_t;

typedef struct behaviour_t {
  player_t *player;
  decision_t last;
  unsigned short raises;
  unsigned short bets;
  unsigned short calls;
  unsigned short checks;
  unsigned short folds;
  unsigned short raises_total;
  unsigned short bets_total;
  unsigned short calls_total;
  unsigned short checks_total;
  unsigned short folds_total;
} behaviour_t;

extern behaviour_t behaviour[PLAYERS_COUNT];

typedef struct ai_t {
  ai_diff_t type;
  unsigned short strength;
  unsigned short potential;

                  /* AI parameters */
  byte_t aggr;    /* aggressiveness */
  byte_t rnds;    /* randomness */
  byte_t bluf;    /* chance of bluff */
  byte_t tact;    /* tactics (consider the potential) */

  bool_t bluffing;
  bool_t slowplaying;
} ai_t;

decision_t ai_decision(player_t *player);
decision_t ai_decision_dumb(player_t *player);
/*decision_t ai_decision_smarter(player_t *player);*/

unsigned short ai_strength(player_t *player);
void ai_update_behaviour(player_t *p, decision_t d, behaviour_t *b);
void ai_clear_behaviour(behaviour_t *b);
void ai_init_behaviour(behaviour_t *b);

#endif
