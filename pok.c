#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "card.h"
#include "pok.h"

void sort_by_rank(card_t** pool) {
  card_t* temp;
  int i, j;

  for (i = 0; i < POOL_SIZE-1; ++i) {
    for (j = i; j >= 0; j--) {
      if (strchr(RANKS, (*pool[j+1])[RANK]) > strchr(RANKS, (*pool[j])[RANK])) {
        temp = pool[j];
        pool[j] = pool[j+1];
        pool[j+1] = temp;
      }
    }
  }
}

void fill_reps(card_t** pool, byte_t* reps) {
  counter_t i, j;
  byte_t rep;

  for (i = 0; i < POOL_SIZE; ++i) {
    rep = 1;
    for (j = 0; j < POOL_SIZE; ++j)   {
      if (pool[i] != pool[j]) {
        if ( (*pool[i])[RANK] == (*pool[j])[RANK] )
          rep++;
      }
    }
    reps[i] = rep;
  }
}

void fill_dist(card_t** pool, byte_t* dist) {
  counter_t i;

  for (i = 0; i < POOL_SIZE-1; ++i) {
    dist[i] = strchr(RANKS, (*pool[i])[RANK]) - strchr(RANKS, (*pool[i+1])[RANK]);
  }
}

byte_t suit_count(card_t** pool, char suit) {
  counter_t i;
  byte_t count = 0;

  for (i = 0; i < POOL_SIZE; ++i) {
    if ( (*pool[i])[SUIT] == suit)
      count++;
  }

  return count;
}

bool_t four_of_a_kind(card_t** pool, combo_t* result) {
  byte_t reps[POOL_SIZE];
  counter_t i, j;

  fill_reps(pool, reps);

  for (i = 0; i < POOL_SIZE; ++i) {
    if (reps[i] == 4) {
      result->kind = FOUR;
      result->rank[0] = pool[i];
      for (j = 0; j < POOL_SIZE; j++) {
        if (reps[j] != 4) {
          result->kickers[0] = pool[j];
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

bool_t full_house(card_t** pool, combo_t* result) {
  byte_t reps[POOL_SIZE];
  counter_t i, j;

  fill_reps(pool, reps);

  for (i = 0; i < POOL_SIZE; ++i) {
    if (reps[i] == 3) {
      for (j = 0; j < POOL_SIZE; j++) {
        if (   (reps[j] == 2)  ||
             ( (reps[j] == 3) && ((*pool[j])[RANK] != (*pool[i])[RANK]) )
            ) {
          result->kind = FULLH;
          result->rank[0] = pool[i];
          result->rank[1] = pool[j];
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

bool_t flush(card_t** pool, combo_t* result) {
  counter_t i, j;

  for (i = 0; i < strlen(SUITS); ++i) {
    if (suit_count(pool, SUITS[i]) >= 5) {
      result->kind = FLUSH;
      for (j = 0; j < POOL_SIZE; ++j) {
        if ((*pool[j])[SUIT] == SUITS[i]) {
          if (!result->rank[0]) result->rank[0] = pool[j]; else
            if (!result->kickers[0]) result->kickers[0] = pool[j]; else
              if (!result->kickers[1]) result->kickers[1] = pool[j]; else
                if (!result->kickers[2]) result->kickers[2] = pool[j]; else
                  if (!result->kickers[3]) result->kickers[3] = pool[j]; else
                    return TRUE;
        }
      }
    }
  }

  return FALSE;
}

bool_t straight(card_t** pool, combo_t* result) {
  byte_t dist[POOL_SIZE-1];
  byte_t chain;
  counter_t i;
  card_t* first;

  fill_dist(pool, dist);

  chain = 0;
  for (i = 0; i < POOL_SIZE-1; ++i) {
    if (!chain) first = pool[i];
    if ((dist[i] == 1) || (dist[i] == 0)) {
      chain += dist[i];
      if (chain >= 4) {
        result->kind = STRAIGHT;
        result->rank[0] = first;
        return TRUE;
      }
    } else
      chain = 0;
  }

  return FALSE;
}

/* NOT true */
bool_t straight_flush(card_t** pool, combo_t* result) {
  if (flush(pool, result)) {
    if (straight(pool, result)) {
      result->kind = STRAIGHTF;
      return TRUE;
    }
  }

  return FALSE;
}

bool_t three_of_a_kind(card_t** pool, combo_t* result) {
  byte_t reps[POOL_SIZE];
  counter_t i, j;

  fill_reps(pool, reps);

  for (i = 0; i < POOL_SIZE; ++i) {
    if (reps[i] == 3) {
      result->kind = THREE;
      result->rank[0] = pool[i];
      for (j = 0; j < POOL_SIZE; j++) {
        if (reps[j] != 3) {
          if (!result->kickers[0]) result->kickers[0] = pool[j]; else
            if (!result->kickers[1]) result->kickers[1] = pool[j]; else
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

bool_t pairs(card_t** pool, combo_t* result) {
  byte_t reps[POOL_SIZE];
  counter_t i, j;
  card_t* first = NULL;

  fill_reps(pool, reps);

  for (i = 0; i < POOL_SIZE; ++i) {
    if (reps[i] == 2) {
      if (!first) {
        first = pool[i];
      } else {
        if ((*pool[i])[RANK] != (*first)[RANK]) {
          result->kind = PAIRS;
          result->rank[0] = first;
          result->rank[1] = pool[i];
          for (j = 0; j < POOL_SIZE; j++) {
            if (   (reps[j] != 2)  ||
                 ( (reps[j] == 2)
                  && ((*pool[j])[RANK] != (*pool[i])[RANK])
                  && ((*pool[j])[RANK] != (*first)[RANK]) )
                ) {
              result->kickers[0] = pool[j];
              return TRUE;
            }
          }
        }
      }
    }
  }

  return FALSE;
}

bool_t pair(card_t** pool, combo_t* result) {
  byte_t reps[POOL_SIZE];
  counter_t i, j;

  fill_reps(pool, reps);

  for (i = 0; i < POOL_SIZE; ++i) {
    if (reps[i] == 2) {
      result->kind = PAIR;
      result->rank[0] = pool[i];
      for (j = 0; j < POOL_SIZE; j++) {
        if (reps[j] != 2) {
          if (!result->kickers[0]) result->kickers[0] = pool[j]; else
            if (!result->kickers[1]) result->kickers[1] = pool[j]; else
             if (!result->kickers[2]) result->kickers[2] = pool[j]; else
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

combo_t pok_resolve(card_t** pool) {
  combo_t result;
  int i;

  for (i = 0; i < KICKERS_COUNT; ++i)
    result.kickers[i] = NULL;
  for (i = 0; i < RANK_CARDS_COUNT; ++i)
    result.rank[i] = NULL;

  sort_by_rank(pool);

  if (straight_flush(pool, &result))
    return result;
  if (four_of_a_kind(pool, &result))
    return result;
  if (full_house(pool, &result))
    return result;
  if (flush(pool, &result))
    return result;
  if (straight(pool, &result))
    return result;
  if (three_of_a_kind(pool, &result))
    return result;
  if (pairs(pool, &result))
    return result;
  if (pair(pool, &result))
    return result;

  result.kind = HIGHC;
  result.rank[0] = pool[0];
  for (i = 0; i < KICKERS_COUNT; ++i) {
    result.kickers[i] = pool[i+1];
  }

  return result;
}

card_t** pok_compare(card_t** pool1, card_t** pool2) {
  counter_t i;
  char* rank1, *rank2;

  combo_t result1 = pok_resolve(pool1);
  combo_t result2 = pok_resolve(pool2);

  if (result1.kind > result2.kind)
    return pool1;
  if (result1.kind < result2.kind)
    return pool2;
  if (result1.kind == result2.kind) {
    for (i = 0; i < RANK_CARDS_COUNT; ++i) {
      if (result1.rank[i] && result1.rank[i]) {
        rank1 = strchr(RANKS, (*result1.rank[i])[RANK]);
        rank2 = strchr(RANKS, (*result2.rank[i])[RANK]);
        if (rank1 > rank2)
          return pool1;
        if (rank1 < rank2)
          return pool2;
      }
    }
    for (i = 0; i < KICKERS_COUNT; ++i) {
      if (result1.kickers[i] && result1.kickers[i]) {
        rank1 = strchr(RANKS, (*result1.kickers[i])[RANK]);
        rank2 = strchr(RANKS, (*result2.kickers[i])[RANK]);
        if (rank1 > rank2)
          return pool1;
        if (rank1 < rank2)
          return pool2;
      }
    }
  }

  return NULL;    /* Tie */
}
