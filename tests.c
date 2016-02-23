#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "card.h"
#include "pok.h"
#include "tests.h"

#define CASES_COUNT 1000

static const char* resolve_data[][POOL_SIZE] = {
  {"Qs", "2d", "3s", "Th", "Jc", "7s", "9h"},  /* High card */
  {"Qs", "2d", "3s", "Th", "Jc", "9s", "9h"},  /* Pair */
  {"7s", "7d", "As", "2h", "Tc", "Ts", "8h"},  /* Two pairs */
  {"7s", "7d", "As", "8h", "Tc", "Ts", "8h"},  /* Two pairs (2)*/
  {"7s", "7d", "As", "2h", "8c", "Ts", "7h"},  /* Three */
  {"7s", "8d", "As", "Jh", "9c", "Ts", "Th"},  /* Straight */
  {"7s", "7d", "As", "2s", "7c", "Ts", "8s"},  /* Flush */
  {"7s", "8s", "As", "Js", "9s", "Td", "Th"},  /* Flush (with straight variant) */
  {"As", "7d", "Ad", "Ah", "8c", "7s", "7h"},  /* Full house */
  {"7s", "7d", "As", "Ah", "8c", "Ts", "7h"},  /* Full house (2) */
  {"7s", "7d", "As", "2h", "7c", "Ts", "7h"},  /* Four */
  {"7s", "8s", "As", "Js", "9s", "Ts", "Th"},  /* Straight flush */
  {"2s", "3s", "4s", "5s", "6s", "7d", "8s"}   /* Straight flush */
};

void clear_pool(card_t** pool) {
  int i;
  for (i = 0; i < POOL_SIZE; ++i) pool[i] = NULL;
}

void print_pool(card_t** pool, const size_t pool_size) {
  int i;
  for (i = 0; i < pool_size; ++i) {
    if (pool[i]) {
      printf("%c%c ", (*pool[i])[RANK], (*pool[i])[SUIT]);
    }
  }
}

void randomize_pool(card_t** pool, card_t* pool_cards) {
  int i;
  for (i = 0; i < POOL_SIZE; ++i) {
    card_random_unique(pool_cards[i], (const card_t**)pool, POOL_SIZE);
    pool[i] = &pool_cards[i];
  }
}

combo_t resolve_pool(card_t** pool, byte_t pool_size) {
  combo_t result;
  char str[255];

  result = pok_resolve(pool, pool_size);

  printf(" %s, rank ", card_kind_to_text(result.kind));
  print_pool(result.rank, RANK_CARDS_COUNT);
  printf(" kickers ");
  print_pool(result.kickers, KICKERS_COUNT);
  printf("\n");
  card_combo_to_text(str, result);
  printf("card_combo_to_text: %s\n", str);

  return result;
}

void test(void) {

  card_t pool_cards[POOL_SIZE];
  card_t* pool[POOL_SIZE];
  card_t pool_cards2[POOL_SIZE];
  card_t* pool2[POOL_SIZE];
  card_t** compare_result;
  int i, j;
  byte_t random_size;

  printf("\nresolve tests:\n\n");

  printf("\nrandom pools:\n");

  for (i = 0; i < CASES_COUNT; ++i) {
    clear_pool(pool);
    randomize_pool(pool, pool_cards);
    printf("pool: "); print_pool(pool, POOL_SIZE);
    resolve_pool(pool, POOL_SIZE);
  }

  printf("\npre-defined pools:\n\n");

  for (i = 0; i < (sizeof(resolve_data)/(sizeof(char*))) / POOL_SIZE; i++) {
   for (j = 0; j < POOL_SIZE; ++j) {
      card_fill(pool_cards[j], resolve_data[i][j]);
      pool[j] = &pool_cards[j];
    }
    printf("pool: "); print_pool(pool, POOL_SIZE);
    sort_by_rank(pool, POOL_SIZE);
    printf(" sorted: "); print_pool(pool, POOL_SIZE);
    resolve_pool(pool, POOL_SIZE);
  }

  printf("\ncomparison tests:\n\n");

  for (i = 0; i < CASES_COUNT; ++i) {
    clear_pool(pool);
    randomize_pool(pool, pool_cards);
    printf("pool1: "); print_pool(pool, POOL_SIZE);
    resolve_pool(pool, POOL_SIZE);
    clear_pool(pool2);
    randomize_pool(pool2, pool_cards2);
    printf("pool2: "); print_pool(pool2, POOL_SIZE);
    resolve_pool(pool2, POOL_SIZE);

    compare_result = pok_compare(pool, pool2);
    if (compare_result == pool)
      printf("pool 1 is stronger\n");
    else if (compare_result == pool2)
      printf("pool 2 is stronger\n");
    else {
      printf("tie!\n");
    }
  }

  printf("\nincomplete pools:\n\n");

  for (i = 0; i < CASES_COUNT; ++i) {
    random_size = rand() % POOL_SIZE + 1;
    clear_pool(pool);
    randomize_pool(pool, pool_cards);
    printf("pool: "); print_pool(pool, random_size);
    resolve_pool(pool, random_size);
  }

  /*while(1) {
    clear_pool(pool);
    randomize_pool(pool, pool_cards);
    result = pok_resolve(pool);
    sort_by_rank(pool);
    if (result.kind == STRAIGHTF) {
      print_pool(pool, POOL_SIZE);
      resolve_pool(pool);
    }
  }*/

}
