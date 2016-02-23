#include <string.h>
#include <stdlib.h>

#include "defs.h"
#include "card.h"
#include "pok.h"

bool_t card_validate(const card_t card) {
  char rank = card[RANK];
  char suit = card[SUIT];

  return (strchr(RANKS, rank) && strchr(SUITS, suit));
}

void card_fill(card_t card, const char* str) {
  if (strlen(str) >= 2) {
    card[RANK] = str[RANK];
    card[SUIT] = str[SUIT];
  } else
    card[RANK] = card[SUIT] = EMPTY_CARD;
}

void card_random(card_t card) {
  unsigned char r_rank = rand() % strlen(RANKS);
  unsigned char r_suit = rand() % strlen(SUITS);

  card[RANK] = RANKS[r_rank];
  card[SUIT] = SUITS[r_suit];
}

void card_random_unique(card_t card, const card_t** pool, const size_t pool_size) {
  bool_t is_unique;
  int i;

  do {
    is_unique = TRUE;
    card_random(card);
    for (i = 0; i < pool_size; ++i) {
      if (pool[i]) {
        if ( (card[RANK] == (*pool[i])[RANK]) &&
             (card[SUIT] == (*pool[i])[SUIT]) ) {
          is_unique = FALSE;
          break;
        }
      }
    }
  } while (!is_unique);
}

combo_t card_resolve(hand_t hand, open_t open) {
  card_t* pool[POOL_SIZE];
  int i;

  for (i = 0; i < HAND_SIZE; ++i)
    pool[i] = hand[i];
  for (i = 0; i < TABLE_SIZE; ++i)
    pool[HAND_SIZE + i] = open[i];

  return pok_resolve(pool, POOL_SIZE);
}

bool_t card_cmp_suits(card_t card1, card_t card2) {
  return (card1[SUIT] == card2[SUIT]);
}

bool_t card_cmp_ranks(card_t card1, card_t card2) {
  return (card1[RANK] == card2[RANK]);
}

char* card_kind_to_text(hand_kind_t kind) {
  switch (kind) {
    case HIGHC: return "Hign card"; break;
    case PAIR: return "Pair"; break;
    case PAIRS: return "Two pairs"; break;
    case THREE: return "Three of a kind"; break;
    case STRAIGHT: return "Straight"; break;
    case FLUSH: return "Flush"; break;
    case FULLH: return "Full house"; break;
    case FOUR: return "Four of a kind"; break;
    case STRAIGHTF: return "Straight flush"; break;
  }
  return NULL;
}

void card_copy(card_t from, card_t to) {
  to[RANK] = from[RANK];
  to[SUIT] = from[SUIT];
}

void card_combo_to_text(char* str, combo_t combo) {
  char kickers_line[32] = "";
  char rank_line[8] = "";
  counter_t i;

  if (combo.kickers[0]) {
    strcat(kickers_line, ", kicker(s): ");
    for(i = 0; i < KICKERS_COUNT; i++)
      if (combo.kickers[i]) {
        strncat(kickers_line, *combo.kickers[i], 2);
        strcat(kickers_line, " ");
      }
  }

  if (combo.rank[1])
    sprintf(rank_line, "(%c, %c)", (*combo.rank[0])[RANK], (*combo.rank[1])[RANK]);
  else
    sprintf(rank_line, "(%c)", (*combo.rank[0])[RANK]);

  sprintf(str, "%s %s%s", card_kind_to_text(combo.kind), rank_line, kickers_line);
}
