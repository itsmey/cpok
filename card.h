#ifndef CARD_H
#define CARD_H

#include "defs.h"

#define EMPTY_CARD '-'
#define KICKERS_COUNT 4
#define RANK_CARDS_COUNT 2

typedef char card_t[2];
typedef card_t* hand_t[HAND_SIZE];
typedef card_t* open_t[TABLE_SIZE];

typedef enum {HIGHC,
              PAIR,
              PAIRS,
              THREE,
              STRAIGHT,
              FLUSH,
              FULLH,
              FOUR,
              STRAIGHTF} hand_kind_t;

typedef struct combo_t {
  hand_kind_t kind;
  card_t* rank[RANK_CARDS_COUNT];
  card_t* kickers[KICKERS_COUNT];
} combo_t;

bool_t card_validate(const card_t card);
void card_fill(card_t card, const char* str);
void card_random(card_t card);
void card_random_unique(card_t card, const card_t** pool, const size_t pool_size);
combo_t card_resolve(hand_t hand, open_t open);
bool_t card_cmp_suits(card_t card1, card_t card2);
bool_t card_cmp_ranks(card_t card1, card_t card2);
char * card_kind_to_text(hand_kind_t kind);
void card_copy(card_t from, card_t to);

#endif
