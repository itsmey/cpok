#ifndef POK_H
#define POK_H

#include "card.h"

void sort_by_rank(card_t** pool);
combo_t pok_resolve(card_t** pool);
card_t** pok_compare(card_t** pool1, card_t** pool2);


#endif
